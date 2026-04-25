#include <glad/wgl.h>

#include <fg/graphics/opengl/gl.h>
#include "fg/utils/log.h"
#include "fg/window/registry.h"
#include "fg/window/win32/utils/console.h"
#include "fg/window/win32/window_impl.h"
#include "fg/window/win32/window_proc.h"
#include "fg/window/window_internal.h"

namespace fg::window::win32 {
namespace {
bool GLInit(HWND hwnd, HDC hdc);
void FetchEvent();
}  // namespace

Win32WindowImpl::Win32WindowImpl(const WindowConfig& config, WindowBase* base) {
    utils::CreateConsole();

    WindowState* state = GetMutableState(base);
    window_id_ = state->id;

    HINSTANCE hInstance = GetModuleHandleW(NULL);
    const wchar_t class_name[] = L"WindowClass";
    WNDCLASSW wc = {};

    wc.hInstance = hInstance;
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = class_name;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // 默认背景色
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);       // 默认光标
    wc.style = CS_DBLCLKS;

    if (!RegisterClassW(&wc)) {
        MessageBoxW(
          NULL, L"register class window error", L"Error", MB_ICONERROR);
        exit(1);
    }

    create_window_ready_flag_ = false;
    hwnd_ = CreateWindowExW(
      0,  // style
      wc.lpszClassName,
      config.name,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      config.width,
      config.height,
      NULL,  // 没有 parent
      NULL,  // 暂时没有 menu
      hInstance,
      this  // 直接传入 Win32WindowImpl* 就行了，主要是为了处理退出逻辑
    );
    create_window_ready_flag_ = true;

    state->height = config.height;
    state->width = config.width;

    // 获取窗口 hdc，记得 release (这个操作在 window proc)
    hdc_ = GetDC(hwnd_);

    if (!GLInit(hwnd_, hdc_)) {
        MessageBoxW(NULL, L"GLinit error", L"Error", MB_ICONERROR);
    }

    WindowRegisterInfo reg_info;
    reg_info.id = window_id_;
    reg_info.window = base;
    RegisterWindow(reg_info);

    event::SetFetchEvent(FetchEvent);

    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);  // 这个接口是让 WM_PAINT 消息提高优先级
}

Win32WindowImpl::~Win32WindowImpl() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
    }
    UnregisterWindow(window_id_);
}

void Win32WindowImpl::SwapBuffer() const { SwapBuffers(hdc_); }

HDC Win32WindowImpl::GetHDC() const { return hdc_; }
HWND Win32WindowImpl::GetHWND() const { return hwnd_; }
unsigned int Win32WindowImpl::GetID() const { return window_id_; }

namespace {

void FetchEvent() {
    float sys_dispatch_msg_limit = 0.001f;
    MSG msg;
    float peek_begin_time = fg::utils::GetTime();
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) &&
           GlobalEventList.Count() < GlobalEventList.Capacity &&
           fg::utils::GetTime() < peek_begin_time + sys_dispatch_msg_limit) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

bool GLInit(HWND hwnd, HDC hdc) {
    // 设置像素格式，并绑到 hdc
    PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR),  // 结构体大小
      1,                              // 版本号
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
        PFD_DOUBLEBUFFER,  // flags，包括双缓冲
      PFD_TYPE_RGBA,       // RGBA 颜色类型
      32,                  // 32 位颜色深度
      0,
      0,
      0,
      0,
      0,
      0,  // 忽略颜色位
      0,  // 无 alpha 缓冲区
      0,  // 忽略移位位
      0,  // 无累积缓冲区
      0,
      0,
      0,
      0,               // 忽略累积位
      24,              // 24 位深度缓冲区
      8,               // 8 位模板缓冲区
      0,               // 无辅助缓冲区
      PFD_MAIN_PLANE,  // 主层
      0,               // 保留
      0,
      0,
      0  // 忽略层掩码
    };

    // 这个 choose 接口是 windows sdk 提供的
    // opengl 也有可扩展的像素格式，wglChoosePixelFormatARB
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    // 初始化 glad，因为 opengl 要求必须在 rc 非空的情况下才能 load
    // 目标函数，所以我们需要一个 temp rc 创建 temp rendering context
    // wglCreateContext 这个接口只能创建 1.x 版本的 rc
    HGLRC temp_rc = wglCreateContext(hdc);
    if (!temp_rc) {
        VLOG(0) << "temp_rc";
        return false;
    }

    // 设置成当前的渲染设备
    if (!wglMakeCurrent(hdc, temp_rc)) {
        VLOG(0) << "wglMakeCurrent";
        return false;
    }

    // 设置 rc 之后可以加载 extension 函数
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
      (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress(
        "wglCreateContextAttribsARB");
    if (!wglCreateContextAttribsARB) {
        VLOG(0) << "wglGetProcAddress wglCreateContextAttribsARB";
        return false;
    }

    // 创建 opengl 3.3 的 rc，注意，这个创建必须在删除 temp_rc 之前
    // 因为 wglCreateContextAttribsARB 这个函数依赖 temp_rc
    // 存在，如果删掉，下面的创建有问题
    const int attribs[] = {WGL_CONTEXT_MAJOR_VERSION_ARB,
                           3,
                           WGL_CONTEXT_MINOR_VERSION_ARB,
                           3,
                           WGL_CONTEXT_PROFILE_MASK_ARB,
                           WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                           0};

    HGLRC morden_render_ctx = wglCreateContextAttribsARB(hdc, 0, attribs);
    if (!morden_render_ctx) {
        VLOG(0) << "morden_render_ctx";
        return false;
    }

    // 删除 temp rc
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(temp_rc);

    wglMakeCurrent(hdc, morden_render_ctx);

    // 初始化 glad，现在要求 rc 是 3.3+
    if (!gladLoaderLoadWGL(hdc)) {
        VLOG(0) << "gladLoaderLoadWGL";
        return false;
    }
    if (!gladLoaderLoadGL()) {
        VLOG(0) << "gladLoaderLoadGL";
        return false;
    }

    // 加载之后，这里应该能找到 glGetString 符号，不然就是加载有问题
    if (!glGetString) {
        return false;
    }
    const char* version = (const char*)glGetString(GL_VERSION);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    // 这个 log 先留着后面改
    VLOG(0) << "Minimum OpenGL support:\nVersion: " << version
            << "\nRenderer: " << renderer;

    // opengl 的一些基础设置，先设置一下
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    return true;
}
}  // namespace

}  // namespace fg::window::win32
