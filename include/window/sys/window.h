#include<Windows.h>
#include "window/window.h"

// 主要是支持创建，通过 CreateWindowExW 的 lpParam 传递 this
// 使用 SetWindowLongPtr 和 SetProp 是的 hwnd 与 window 结构关联
// 编写一个核心 WindowProc 循环，并且在适当时机调用 user handle
// P.S. SetWindowLongPtr 只能提供一个 user data 存储位（pointer）
// 而 Prop 没有限制，但是更慢

namespace fei_window{

    
class FEI_WindowsWindow : FEI_Window{
    HWND hwnd_;
    
public:
    FEI_WindowsWindow(const wchar_t* window_name, int x, int y);
    ~FEI_WindowsWindow();
};

}
