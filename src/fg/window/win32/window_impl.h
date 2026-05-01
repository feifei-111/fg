#pragma once
#include <Windows.h>

// win32 的 CreateWindow 居然是个宏，在 win32 这里会报错
#ifdef CreateWindow
#undef CreateWindow
#endif

#include <fg/window/window.h>

namespace fg::window {

class Window::WindowImpl {
    HWND hwnd_;
    HDC hdc_;
    unsigned int window_id_;
    bool create_window_ready_flag_;

public:
    WindowImpl(const WindowConfig& config);
    ~WindowImpl();
    void SwapBuffer() const { SwapBuffers(hdc_); }
    HDC GetHDC() const { return hdc_; }
    HWND GetHWND() const { return hwnd_; }
    unsigned int GetID() const { return window_id_; }
    bool IsReady() const { return create_window_ready_flag_; }
};

}  // namespace fg::window
