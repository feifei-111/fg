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
    std::string_view Name() const { return "ExitEvent"; }
};

struct PaintEvent {
    int width;
    int height;
    std::string_view Name() const { return "PaintEvent"; }
};

struct MouseMoveEvent {
    int x;
    int y;
    std::string_view Name() const { return "MouseMoveEvent"; }
};

struct MouseClickEvent {
    Button button;
    ButtonMove move;
    int x;
    int y;
    std::string_view Name() const { return "MouseClickEvent"; }
};

struct MouseWheelEvent {
    int wheel_delta;
    int x;
    int y;
    std::string_view Name() const { return "MouseWheelEvent"; }
};

struct KeyBoardEvent {
    Button button;
    ButtonMove move;
    std::string_view Name() const { return "KeyBoardEvent"; }
};

using EventData = std::variant<ExitEvent,
                               PaintEvent,
                               MouseMoveEvent,
                               MouseClickEvent,
                               MouseWheelEvent,
                               KeyBoardEvent>;

std::string_view FG_API EventDataName(const EventData& data);

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

void FG_API CollectEvents(bool clear_events = true);
bool FG_API PollEvent(Event* event);

}  // namespace fg::window::event
