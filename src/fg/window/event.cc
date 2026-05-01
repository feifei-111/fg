#include <mutex>

#include <fg/window/event.h>
#include <fg/window/window.h>
#include <functional>
#include <type_traits>
#include "fg/utils/log.h"
#include "fg/window/internal.h"

namespace fg::window::event {

using WindowState = fg::window::WindowState;

std::string_view EventDataName(const EventData& data) {
    return std::visit(
      [](auto&& arg) -> std::string_view {
          return std::decay_t<decltype(arg)>::Name();
      },
      data);
}

/* =========================== EventQueue Buffer =========================== */

// 消息循环是全局的，不论有多少线程，都是用同一个消息循环、
// 所以这个 queue 不需要锁，因为只有一个线程会去 fetch msg
struct EventQueue {
public:
    static constexpr unsigned int Capacity = 128;

private:
    unsigned int head_ = 0;
    unsigned int tail_ = 0;
    unsigned int count_ = 0;
    Event events_[Capacity];

    bool in_prepare_event_ = false;

public:
    int Count() { return count_; }

    // 提供两个接口开控制 push，不用拷贝了
    EventData* PrepareNextEventData(unsigned int window_id) {
        CHECK(!in_prepare_event_)
          << "prepare another event when last one not finished";
        if (count_ == Capacity) {
            return nullptr;
        }
        events_[tail_].window_id = window_id;
        events_[tail_].time_stamp = fg::utils::GetTime();
        in_prepare_event_ = true;
        return &(events_[tail_].data);
    }

    void PrepareEventDataDone() {
        CHECK(in_prepare_event_)
          << "try finish a event when no event is preparing";
        VLOG(7) << "PrepareEventDataDone: " << events_[tail_].TypeStr() << ", "
                << events_[tail_].window_id << ", "
                << events_[tail_].time_stamp;
        tail_ = (tail_ + 1) % Capacity;
        count_ += 1;
        in_prepare_event_ = false;
    }

    bool PopHeadEvent(Event& event) {
        if (count_ == 0) {
            return false;
        }
        event = events_[head_];
        head_ = (head_ + 1) % Capacity;
        count_ -= 1;
        return true;
    }

    bool Empty() { return count_ == 0; }

    void Clear() {
        head_ = 0;
        tail_ = 0;
        count_ = 0;
        in_prepare_event_ = false;
    }
};

static EventQueue GlobalEventQueue;

/* ============================ Event Poll ================================ */

namespace {
struct UpdateStateVisitor {
    WindowState& state;
    UpdateStateVisitor(WindowState& target_state) : state(target_state) {}
    void operator()(MouseMoveEvent& event_data) {
        state.mouse_x = event_data.x;
        state.mouse_y = event_data.y;
    }
    void operator()(MouseClickEvent& event_data) {
        state.mouse_x = event_data.x;
        state.mouse_y = event_data.y;
        switch (event_data.move) {
            case ButtonMove::DOWN:
                state.button_map[size_t(event_data.button)] = true;
                break;
            case ButtonMove::UP:
                state.button_map[size_t(event_data.button)] = false;
                break;
        }
    }
    void operator()(MouseWheelEvent& event_data) {
        state.mouse_x = event_data.x;
        state.mouse_y = event_data.y;
    }
    void operator()(KeyBoardEvent& event_data) {
        switch (event_data.move) {
            case ButtonMove::DOWN:
                state.button_map[static_cast<unsigned int>(event_data.button)] =
                  true;
                break;
            case ButtonMove::UP:
                state.button_map[static_cast<unsigned int>(event_data.button)] =
                  false;
                break;
        }
    }
    void operator()(PaintEvent& event_data) {
        state.width = event_data.width;
        state.height = event_data.height;
    }
    // default do nothing
    template <typename T>
    void operator()(T& val) {}
};

void UpdateState(Event& event, WindowState& state) {
    state.time_stamp = event.time_stamp;
    std::visit(UpdateStateVisitor(state), event.data);
}
}  // namespace

// win32 的消息循环以及 glfw 的 poll 都是全局的，多个窗口共用的
// 所以这里用一个全局的设置就可以了
void (*FetchEvent)() = nullptr;

void SetFetchEventHook(void (*hook)()) {
    if (FetchEvent == nullptr) {
        FetchEvent = hook;
    }
}

void CollectEvents(bool clear_events) {
    if (clear_events) {
        GlobalEventQueue.Clear();
    }
    FetchEvent();
}

bool PollEvent(Event& event) {
    if (GlobalEventQueue.PopHeadEvent(event)) {
        std::weak_ptr<Window> window = GetRegisteredWindowPtr(event.window_id);
        UpdateState(event, GetMutableState(window));
        return true;
    } else {
        return false;
    }
}

/* ======================= Emplace Event Tool =========================== */
namespace detail {
EventData* PrepareNextEventData(unsigned int window_id) {
    return GlobalEventQueue.PrepareNextEventData(window_id);
}
void PrepareEventDataDone() { return GlobalEventQueue.PrepareEventDataDone(); }
}  // namespace detail

}  // namespace fg::window::event
