#pragma once

namespace fg_event{

enum EventType{
    CommonWindowsEventType
};

struct CommonWindowsEvent{
    unsigned int win_msg;
    long long wparam;
    long long lparam;
};

union Event{
    EventType type;
    union {
        CommonWindowsEvent common_windows_event;
    } data;
};

bool PollEvent(Event* event);
bool PushEvent(Event* event);

}
