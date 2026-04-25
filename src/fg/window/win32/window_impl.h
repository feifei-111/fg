#pragma once
#include <Windows.h>

// win32 的 CreateWindow 居然是个宏，在 win32 这里会报错
#ifdef CreateWindow
#undef CreateWindow
#endif

#include <fg/window/window.h>

namespace fg::window::win32 {

class Win32WindowImpl {
    HWND hwnd_;
    HDC hdc_;
    unsigned int window_id_;
    bool create_window_ready_flag_;

public:
    Win32WindowImpl(const WindowConfig& config, WindowBase* base);
    void SwapBuffer() const;
    HDC GetHDC() const;
    HWND GetHWND() const;
    unsigned int GetID() const;
    bool IsReady() const { return create_window_ready_flag_; }
    ~Win32WindowImpl();
};

}  // namespace fg::window::win32

namespace fg::window {
using WindowImpl = win32::Win32WindowImpl;
}
