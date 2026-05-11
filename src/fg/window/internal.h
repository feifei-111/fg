#pragma once

#include <fg/window/event.h>
#include <fg/window/window.h>
#include <functional>
#include <utility>

#include "fg/utils/log.h"

// 提供一个 lib 内部可用的 mutable state 获取方式
namespace fg::window {
WindowState& GetMutableState(std::weak_ptr<Window> window);

struct WindowRegisterInfo {
    unsigned int id;
    std::weak_ptr<Window> window;
};

void RegisterWindow(const WindowRegisterInfo& reg_info);
void UnregisterWindow(unsigned int window_id);
std::weak_ptr<Window> GetRegisteredWindowPtr(unsigned int window_id);

namespace event {
void SetFetchEventHook(void (*hook)());

namespace detail {
EventData* PrepareNextEventData(unsigned int window_id);
void PrepareEventDataDone();
}  // namespace detail

template <typename EventDataType, typename... Args>
bool EmplaceEvent(unsigned int window_id, Args&&... args) {
    EventData* event_data = detail::PrepareNextEventData(window_id);
    if (event_data) {
        event_data->emplace<EventDataType>(std::forward<Args>(args)...);
        detail::PrepareEventDataDone();
    } else {
        VLOG(3) << EventDataType::Name() << ", this event is dropped.";
        return false;
    }
    return true;
}

}  // namespace event

}  // namespace fg::window
