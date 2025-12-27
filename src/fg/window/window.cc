#ifdef _WIN32
#include "fg/window/win32/window_impl.h"
#elifdef __APPLE__
#include "fg/window/glfw/window_impl.h"
#endif

#include <fg/window/window.h>
#include <fg/utils/utils.h>

namespace fg::window {

namespace {
    // 对于 inline 函数，在链接时会自动只保留一份，其他舍弃
    // 此外，inline 函数内的 static 变量是不是只有一份呢？
    // 貌似在 c++17 之后才有这个支持，不过我们就是用 17
    // 所以这个其实可以写在头文件里面，不过那样写本身也容易混淆
    unsigned int GetNewWindowIDImpl() {
        static std::atomic<unsigned int> cur_id{0};
        return cur_id++;
    }
}

unsigned int WindowBase::GetNewWindowID(){
    return GetNewWindowIDImpl();
}

WindowState* GetMutableState(WindowBase* window){
    return &(window->state_);
}

std::shared_ptr<WindowBase> CreateWindow(const WindowConfig& config){
#ifdef _WIN32
    return std::make_shared<Window<win32::Win32Window>>(config);
#elifdef __APPLE__
    return std::make_shared<Window<glfw::GLFWWindowImpl>>(config);
#else
    CHECK(false) << "Platform Not Supported";
#endif
}

}  // namespace fg_window
