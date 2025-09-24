#pragma once

namespace fg_interact{

// 像 ExitEvent，PaintEvent 之类的东西，其实不需要携带额外信息，一个 type 足够了
// 所以可以不用定义对应的 struct，在 enum 里面加一个就行

enum EventType{
    kCommonEvent,
    kExitEvent,
    kPaintEvent,
    kMouseEvent,
    kKeyboardEvent,
    kScrollEvent
};

struct CommonEvent{
    unsigned int win_msg;
    long long wparam;
    long long lparam;
};

struct MouseEvent{

};

struct KeyBoardEvent{

};

struct ScrollEvent{

};

union Event{
    EventType type;
    union {
        CommonEvent common_event;
        MouseEvent mouse_event;
        KeyBoardEvent keyboard_event;
        ScrollEvent scroll_event;
    } data;
};

bool PollEvent(Event* event);
bool PushEvent(Event* event);

}
