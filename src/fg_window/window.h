#pragma once
#include <Windows.h>

#include "fg_interact/state.h"

namespace fg_window{

struct WindowData{
    // 反向引用 window
    Window *window;
    fg_interact::WindowState state;
}
    
class Window{
    HWND hwnd_;
    HDC hdc_;
    WindowData data_;
    
public:
    Window(const wchar_t* window_name, int x, int y);
    HDC GetHDC() const;
    HWND GetHWND() const;
    void SwapBuffer() const;
    fg_interact::WindowState& GetState();
    ~Window();
};

}
