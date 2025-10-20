#pragma once
#ifdef _WIN32

#include <Windows.h>

#include "fg_interact/event.h"
#include "fg_interact/state.h"

#include "fg_macros.h"

namespace fg_window {

class FG_API Window;

struct WindowProp {
    size_t id;
    int height;
    int width;
};

struct WindowData {
    // 反向引用 window
    Window* window;
    WindowProp prop;
    fg_interact::WindowState state;
};

class Window {
    HWND hwnd_;
    HDC hdc_;
    WindowData data_;

public:
    Window(const wchar_t* window_name, int x, int y);
    HDC GetHDC() const;
    HWND GetHWND() const;
    void SwapBuffer() const;
    size_t GetID() const;
    const fg_interact::WindowState* GetState() const;
    ~Window();
};

#endif
}
