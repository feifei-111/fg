// 在这里定义我们要处理的 event
// 还要确定处理 event 的接口的形式
// event 应该有很多种，这样会方便一点
// 我们在基类提供一个默认操作

#include<WIndows.h>

namespace fei_window{


class FEI_CommonWindowsEvent{
    UINT win_msg_type;
    WPARAM wparam;
    LPARAM lparam;
};

union FEI_Event{
    FEI_CommonWindowsEvent common_windows_event;
};

FEI_Event* poll_event();

}
