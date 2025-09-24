#include <iostream>

#include "fg_window/window.h"
#include "fg_window/window_proc.h"
#include "fg_gl/gl.h"

namespace fg_window{

Window::Window(const wchar_t* window_name, int width, int height): data_(){

    HINSTANCE hInstance = GetModuleHandleW(NULL);
    const wchar_t class_name[] = L"WindowClass";
    WNDCLASSW wc = {};

    wc.hInstance = hInstance;
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = class_name;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 默认背景色
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      // 默认光标

    if (!RegisterClassW(&wc)) {
        MessageBoxW(NULL, L"register class window error", L"Error", MB_ICONERROR);
        exit(1);
    }

    HWND hwnd = CreateWindowExW(
        0,                          // style
        wc.lpszClassName,
        window_name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        width, 
        height,
        NULL,                       // 没有 parent
        NULL,                       // 暂时没有 menu
        hInstance, 
        &data_                        // user data，传我们的 data_
    );
    hwnd_ = hwnd;

    // 获取窗口 hdc，记得 release (这个操作在 window proc)
    HDC hdc = GetDC(hwnd);
    hdc_ = hdc;

    if (!fg_gl::GLInit(hwnd, hdc)){
        MessageBoxW(NULL, L"GLinit error", L"Error", MB_ICONERROR);
    }

    data_.window = this;
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);         // 这个接口是让 WM_PAINT 消息提高优先级
}

HDC Window::GetHDC() const{
    return hdc_;
}
HWND Window::GetHWND() const{
    return hwnd_;
}

Window::~Window(){
    if(hwnd_){
        DestroyWindow(hwnd_);
    }
}

void Window::SwapBuffer() const {
    SwapBuffers(hdc_);
}

fg_interact::WindowState& GetState(){
    return data_.state;
}

}