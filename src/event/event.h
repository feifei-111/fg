#include<WIndows.h>

namespace fei_window{


struct CommonWindowsEvent{
    UINT win_msg;
    WPARAM wparam;
    LPARAM lparam;
};

union Event{
    CommonWindowsEvent common_windows_event;
};

bool PollEvent(Event* event);
bool PushEvent(Event* event);

}
