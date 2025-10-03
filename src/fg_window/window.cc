#include <iostream>

#include "fg_window/window.h"
#include "fg_window/window_proc.h"
#include "fg_gl/gl.h"

namespace fg_window{

static size_t GetNewWindowID(){
    static size_t cur_id = 0;
    return cur_id++;
}

Window::Window(const wchar_t* window_name, int width, int height): data_(){
    data_.prop.id = GetNewWindowID();

    HINSTANCE hInstance = GetModuleHandleW(NULL);
    const wchar_t class_name[] = L"WindowClass";
    WNDCLASSW wc = {};

    wc.hInstance = hInstance;
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = class_name;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 默认背景色
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);      // 默认光标
    wc.style = CS_DBLCLKS;

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

    fg_interact::WindowRegInfo reg_info;
    reg_info.window_id = data_.prop.id;
    reg_info.window = (void*) this;
    reg_info.state = &(data_.state);
    fg_interact::RegisterWindow(reg_info);

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
    fg_interact::UnregisterWindow(data_.prop.id);
}

void Window::SwapBuffer() const {
    SwapBuffers(hdc_);
}

const fg_interact::WindowState* Window::GetState() const {
    return &data_.state;
}

size_t Window::GetID() const{
    return data_.prop.id;
}

}