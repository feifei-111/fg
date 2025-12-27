#pragma once
#include <Windows.h>
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
    ~Win32WindowImpl();
};

}  // namespace fg::window::win32
