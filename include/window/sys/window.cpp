#include "window/sys/window.h"

namespace fei_window{

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

FEI_WindowsWindow::FEI_WindowsWindow(const wchar_t* window_name, int size_x, int size_y){

    HINSTANCE hInstance = GetModuleHandleW(NULL);
    const wchar_t class_name[] = TEXT("FEI_WindowClass");
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
}


}