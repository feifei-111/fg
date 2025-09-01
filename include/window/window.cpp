#include "window/window.h"
#include "event/event.h"

namespace fei_window{

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0); // 退出消息循环
            return 0;
        case WM_PAINT: {
            // windows 要求 WM_PAINT 必须用 BeginPaint 和 EndPaint
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            TextOutW(hdc, 50, 50, L"TEST", 30);
            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            Event event;
            event.common_windows_event.win_msg = uMsg;
            event.common_windows_event.wparam = wParam;
            event.common_windows_event.lparam = lParam;
            PushEvent(&event);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam); // 默认消息处理
    }
}

Window::Window(const wchar_t* window_name, int size_x, int size_y){

    HINSTANCE hInstance = GetModuleHandleW(NULL);
    const wchar_t class_name[] = TEXT("WindowClass");
    WNDCLASSW wc;

    wc.hInstance = hInstance;
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = class_name;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 默认背景色
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);      // 默认光标

    if (!RegisterClassW(&wc)) {
        MessageBoxW(NULL, TEXT("register class window error"), TEXT("Error"), MB_ICONERROR);
        exit(1);
    }

    HWND hwnd_ = CreateWindowExW(
        0,                          // style
        wc.lpszClassName,
        window_name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 
        CW_USEDEFAULT, 
        size_x, 
        size_y,
        NULL,                       // 没有 parent
        NULL,                       // 暂时没有 menu
        hInstance, 
        NULL                        // user data，我们这里不用
    );

    if (!hwnd_) {
        MessageBoxW(NULL, L"CreateWindowExW error", L"Error", MB_ICONERROR);
        exit(1);
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);         // 这个接口是让 WM_PAINT 消息提高优先级
}

Window::~Window(){}

}