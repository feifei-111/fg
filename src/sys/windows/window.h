#include<Windows.h>

// 主要是支持创建，通过 CreateWindowExW 的 lpParam 传递 this
// 使用 SetWindowLongPtr 和 SetProp 是的 hwnd 与 window 结构关联
// 编写一个核心 WindowProc 循环，并且在适当时机调用 user handle
// P.S. SetWindowLongPtr 只能提供一个 user data 存储位（pointer）
// 而 Prop 没有限制，但是更慢

class FeiWindowsWindow{
    WNDCLASSW wc;
    HINSTANCE module_handle;
    HWND hwnd;
    
public:
    FeiWindowsWindow();
    ~FeiWindowsWindow();
    WNDCLASSW* get_wc();
    void create_window();
    void flush_image();                                                         
};