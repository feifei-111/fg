#pragma once

#include <string>
#include <variant>

#include <fg/macros.h>
#include <fg/utils/utils.h>
#include <fg/window/button.h>

namespace fg::window::event {

using Button = fg::window::Button;
using ButtonMove = fg::window::ButtonMove;

// 不需要额外信息
struct ExitEvent {
    static std::string_view Name() { return "ExitEvent"; }
};

struct PaintEvent {
    int width;
    int height;
    static std::string_view Name() { return "PaintEvent"; }
};

struct MouseMoveEvent {
    int x;
    int y;
    static std::string_view Name() { return "MouseMoveEvent"; }
};

struct MouseClickEvent {
    Button button;
    ButtonMove move;
    int x;
    int y;
    static std::string_view Name() { return "MouseClickEvent"; }
};

struct MouseWheelEvent {
    int wheel_delta;
    int x;
    int y;
    static std::string_view Name() { return "MouseWheelEvent"; }
};

struct KeyBoardEvent {
    Button button;
    ButtonMove move;
    static std::string_view Name() { return "KeyBoardEvent"; }
};

using EventData = std::variant<ExitEvent,
                               PaintEvent,
                               MouseMoveEvent,
                               MouseClickEvent,
                               MouseWheelEvent,
                               KeyBoardEvent>;

FG_API std::string_view EventDataName(const EventData& data);

struct Event {
    Event() {}
    Event(unsigned int id) {
        window_id = id;
        time_stamp = fg::utils::GetTime();
    }
    Event(const Event& other_event) {
        time_stamp = other_event.time_stamp;
        data = other_event.data;
        window_id = other_event.window_id;
    }
    std::string_view TypeStr() const { return EventDataName(data); }
    unsigned int window_id;
    float time_stamp;
    EventData data;
};

FG_API void CollectEvents(bool clear_events = true);
FG_API bool PollEvent(Event& event);

}  // namespace fg::window::event
