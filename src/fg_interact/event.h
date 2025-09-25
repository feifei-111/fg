#pragma once
#include <fg_interact/state.h>
#include <fg_utils/time.h>

namespace fg_interact{

// 像 ExitEvent，PaintEvent 之类的东西，其实不需要携带额外信息，一个 type 足够了
// 所以可以不用定义对应的 struct，在 enum 里面加一个就行

enum EventType{
    kCommonEvent,
    kExitEvent,
    kPaintEvent,
    kMouseEvent,
    kKeyboardEvent,
    kMouseWheelEvent
};

struct CommonEvent{
    unsigned int win_msg;
    long long wparam;
    long long lparam;
};

struct MouseEvent{
    enum Button{
        RButton,
        LButton,
        MButton,
        X1Button,
        X2Button
    };
    enum Move{
        Down,
        Up
    };
    Button button;
    Move move;
    MouseState mouse_state;
};

struct KeyBoardEvent{

};

struct MouseWheelEvent{
    int wheel_delta;
    MouseState mouse_state;
};

union Event{
    Event(){
        time_stamp = fg_utils::GetTime();
    }
    EventType type;
    float time_stamp;
    union {
        CommonEvent common_event;
        MouseEvent mouse_event;
        KeyBoardEvent keyboard_event;
        MouseWheelEvent mouse_wheel_event;
    } data;
};

bool PollEvent(Event* event);
bool PushEvent(Event* event);

}
