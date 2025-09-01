#include <Windows.h>

namespace fei_window{

class WindowData {
    HWND hwnd;
    // 记录鼠标当前位置之类的
    // 这个东西的核心作用在于方便过滤一些 msg，不要都生成 event
};
    
class Window{
    HWND hwnd_;
    WindowData data_;
    
public:
    Window(const wchar_t* window_name, int x, int y);
    ~Window();
};

}
