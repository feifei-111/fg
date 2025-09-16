extern "C"{
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#define GLAD_WGL_IMPLEMENTATION
#include <glad/wgl.h> 
#undef GLAD_WGL_IMPLEMENTATION
}

#include "window/window.h"
#include "event/event.h"

#include<stdio.h>

namespace fei_window{

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    WindowData* data;
    data = (WindowData*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (data->window->hwnd_ != hwnd){
        MessageBoxW(NULL, L"hwnd not correct!", L"Error", MB_ICONERROR);
        exit(1);
    }
    
    switch (uMsg) {
        case WM_NCCREATE:
            data = (WindowData*)(((CREATESTRUCT*)lParam)->lpCreateParams);
            if (!data){
                MessageBoxW(NULL, L"window data NULL!", L"Error", MB_ICONERROR);
                exit(1);
            }
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)data);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);;
        case WM_DESTROY:
            // 增加 event，退出外层循环
            ReleaseDC(hwnd, data->window->hdc_);
            PostQuitMessage(0); // 退出消息循环
            return 0;
        case WM_PAINT: {
            // windows 要求 WM_PAINT 必须用 BeginPaint 和 EndPaint
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            data = (WindowData*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
            if(data) {
                printf("data test: %d\n", data->test);
            }
            Event event;
            event.common_windows_event.win_msg = uMsg;
            event.common_windows_event.wparam = wParam;
            event.common_windows_event.lparam = lParam;
            PushEvent(&event);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam); // 默认消息处理
    }
}

Window::Window(const wchar_t* window_name, int size_x, int size_y): data_(){

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
        size_x, 
        size_y,
        NULL,                       // 没有 parent
        NULL,                       // 暂时没有 menu
        hInstance, 
        &data_                        // user data，传我们的 data_
    );

    if (!hwnd) {
        MessageBoxW(NULL, L"CreateWindowExW error", L"Error", MB_ICONERROR);
        exit(1);
    }

    hwnd_ = hwnd;

    // 获取窗口 hdc，记得 release
    HDC hdc = GetDC(hwnd);
    if(!hdc){
        printf("zzz");
        exit(1);
    }

    hdc_ = hdc;

    // 设置像素格式，并绑到 hdc
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), // 结构体大小
        1,                            // 版本号
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // flags，包括双缓冲
        PFD_TYPE_RGBA,                 // RGBA 颜色类型
        32,                            // 32 位颜色深度
        0, 0, 0, 0, 0, 0,              // 忽略颜色位
        0,                             // 无 alpha 缓冲区
        0,                             // 忽略移位位
        0,                             // 无累积缓冲区
        0, 0, 0, 0,                    // 忽略累积位
        24,                            // 24 位深度缓冲区
        8,                             // 8 位模板缓冲区
        0,                             // 无辅助缓冲区
        PFD_MAIN_PLANE,                // 主层
        0,                             // 保留
        0, 0, 0                        // 忽略层掩码
    };

    // 这个 choose 接口是 windows sdk 提供的
    // opengl 也有可扩展的像素格式，wglChoosePixelFormatARB
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    // 初始化 glad，因为 opengl 要求必须在 rc 非空的情况下才能 load 目标函数，所以我们需要一个 temp rc
    // 创建 temp rendering context
    // wglCreateContext 这个接口只能创建 1.x 版本的
    HGLRC temp_rc = wglCreateContext(hdc);
    if(!temp_rc){
        MessageBoxW(NULL, L"wglCreateContext", L"Error", MB_ICONERROR);
        exit(1);
    }
    // 设置成当前的渲染设备
    if(!wglMakeCurrent(hdc, temp_rc)){
        MessageBoxW(NULL, L"wglMakeCurrent temp", L"Error", MB_ICONERROR);
        exit(1);
    }

    // =========================================================
    // 现在可以加载 extension 函数


    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = 
    (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    if(!wglCreateContextAttribsARB){
        MessageBoxW(NULL, L"wglGetProcAddress 111", L"Error", MB_ICONERROR);
        exit(1);
    }

    // 创建 opengl 4.6 的 rc，注意，这个创建必须在删除 temp_rc 之前
    // 因为 wglCreateContextAttribsARB 这个函数依赖 temp_rc 存在，如果删掉，下面的创建有问题
    const int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    HGLRC morden_render_ctx = wglCreateContextAttribsARB(hdc, 0, attribs);
    if(!morden_render_ctx){
        MessageBoxW(NULL, L"wglCreateContextAttribsARB 222", L"Error", MB_ICONERROR);
        exit(1);
    }
    // =========================================================


    // 删除临时 rc
    if(!wglMakeCurrent(NULL, NULL)){
        MessageBoxW(NULL, L"wwwglad init error", L"Error", MB_ICONERROR);
        exit(1);
    }
    wglDeleteContext(temp_rc);

    if(!wglMakeCurrent(hdc, morden_render_ctx)){
        MessageBoxW(NULL, L"333", L"Error", MB_ICONERROR);
        exit(1);
    }

    // 初始化 glad 和 wglad，要求当前的上下文是 4.6 的，前面不能正确 load
    if (!gladLoaderLoadWGL(hdc)){
        MessageBoxW(NULL, L"wwwglad init error", L"Error", MB_ICONERROR);
        exit(1);
    }
    if (!gladLoaderLoadGL()){
        MessageBoxW(NULL, L"glad init error", L"Error", MB_ICONERROR);
        exit(1);
    }

    // 打印信息
    if (!glGetString){
        MessageBoxW(NULL, L"can not get glGetString", L"Error", MB_ICONERROR);
        exit(1);
    }
    const char* version = (const char*)glGetString(GL_VERSION);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    printf("Minimum OpenGL support:\nVersion: %s\nRenderer: %s\n", version, renderer);

    glViewport(0, 0, size_x, size_y);
    glClearColor(1.0, 0.0, 0.0, 1.0);

    data_.window = this;
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);         // 这个接口是让 WM_PAINT 消息提高优先级
}

Window::~Window(){
    if(hwnd_){
        DestroyWindow(hwnd_);
    }
}

void Window::SwapBuffer(){
    SwapBuffers(hdc_);
}

}