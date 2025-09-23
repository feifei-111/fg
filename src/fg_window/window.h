#pragma once
#include <Windows.h>

namespace fg_window{

class Window;

struct WindowData {
    // 反向引用 window
    Window *window;

    // 记录鼠标当前位置之类的
    // 这个东西的核心作用在于方便过滤一些 msg，不要都生成 event
    int width;
    int height;
    
};
    
class Window{
    HWND hwnd_;
    HDC hdc_;
    WindowData data_;
    
public:
    Window(const wchar_t* window_name, int x, int y);
    HDC GetHDC() const;
    HWND GetHWND() const;
    void SwapBuffer() const;
    ~Window();
};

}
