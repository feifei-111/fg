#include <Windows.h>

namespace fei_window{

class Window;

struct WindowData {
    // 反向引用 window
    Window *window;

    // 记录鼠标当前位置之类的
    // 这个东西的核心作用在于方便过滤一些 msg，不要都生成 event
    int test = 99;
    
};
    
class Window{
    HWND hwnd_;
    HDC hdc_;
    WindowData data_;
    
public:
    Window(const wchar_t* window_name, int x, int y);
    void SwapBuffer();
    ~Window();
};

}
