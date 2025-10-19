#pragma once
#include "fg_interact/state.h"
#include "fg_utils/utils.h"
#include "fg_macros.h"

namespace fg_interact{

// 像 ExitEvent，PaintEvent 之类的东西，其实不需要携带额外信息，一个 type 足够了
// 所以可以不用定义对应的 struct，在 enum 里面加一个就行

enum EventType{
    kCommonEvent,
    kExitEvent,
    kPaintEvent,
    kMouseMoveEvent,
    kMouseClickEvent,
    kMouseWheelEvent,
    kKeyboardEvent
};

struct CommonEvent{
    unsigned int win_msg;
    long long wparam;
    long long lparam;
};

struct MouseMoveEvent{
    int x;
    int y;
};

struct MouseClickEvent{
    Button button;
    ButtonMove move;
    int x;
    int y;
};

struct MouseWheelEvent{
    int wheel_delta;
    int x;
    int y;
};

struct KeyBoardEvent{
    Button button;
    ButtonMove move;
};

struct Event{
    Event(){}
    Event(size_t id) {
        window_id = id;
        time_stamp = fg_utils::GetTime();
    }
    Event(const Event& other_event){
        type = other_event.type;
        time_stamp = other_event.time_stamp;
        data = other_event.data;
        window_id = other_event.window_id;
    }
    size_t window_id;
    float time_stamp;
    EventType type;
    union {
        CommonEvent common_event;
        MouseMoveEvent mouse_move_event;
        MouseClickEvent mouse_click_event;
        MouseWheelEvent mouse_wheel_event;
        KeyBoardEvent keyboard_event;
    } data;
};

bool FG_API PollEvent(Event* event);
bool FG_API PushEvent(Event* event);

struct WindowRegInfo{
    size_t window_id;
    void* window;
    WindowState* state;
};

void FG_API RegisterWindow(const WindowRegInfo& reg_info);
void FG_API UnregisterWindow(size_t window_id);

}
