
#include "fg_window/window.h"
#include "fg_event/event.h"
#include "fg_gl/gl.h"

#include <iostream>

namespace fg_window{

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

    // std::cout << uMsg << ", " << wParam << ", " <<  lParam << std::endl;

    WindowData* data;
    data = (WindowData*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    if (data && data->window && data->window->GetHWND() != hwnd){
        MessageBoxW(NULL, L"hwnd not correct!", L"Error", MB_ICONERROR);
    }
    
    switch (uMsg) {
        case WM_NCCREATE:

            data = (WindowData*)(((CREATESTRUCT*)lParam)->lpCreateParams);
            if (!data){
                MessageBoxW(NULL, L"window data init error!", L"Error", MB_ICONERROR);
                exit(1);
            }
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)data);
            std::cout << "WM_NCCREATE default ret: " << DefWindowProcW(hwnd, uMsg, wParam, lParam) << std::endl;
            // while(1){}
            return 1;
        case WM_DESTROY:
            // 增加 event，退出外层循环
            if (!data || !data->window){
                MessageBoxW(NULL, L"window data init error!", L"Error", MB_ICONERROR);
                exit(1);
            }
            ReleaseDC(hwnd, data->window->GetHDC());
            PostQuitMessage(0); // 退出消息循环
            return 0;
        case WM_PAINT: {
            // 需要重置 Viewport
            // 我们不考虑局部重绘，直接调 GetWindowRect

            RECT window_rect;
            GetWindowRect(hwnd, &window_rect);
            int width = window_rect.right -  window_rect.left;
            int height = window_rect.bottom - window_rect.top;
            glViewport(0, 0, width, height);

            if (data){
                data->height = height;
                data->width = width;
            }

            // windows 要求 WM_PAINT 必须用 BeginPaint 和 EndPaint 局部重绘
            // 如果要用，写这里备忘
            // PAINTSTRUCT ps;
            // HDC hdc = BeginPaint(hwnd, &ps);
            // EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            fg_event::Event event;
            event.data.common_windows_event.win_msg = uMsg;
            event.data.common_windows_event.wparam = wParam;
            event.data.common_windows_event.lparam = lParam;
            PushEvent(&event);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam); // 默认消息处理
    }
}

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

    // data_.width = width;
    // data_.height = height;

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

}