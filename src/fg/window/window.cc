#ifdef _WIN32
#include "fg/window/win32/window_impl.h"
#elif defined(__APPLE__)
#include "fg/window/glfw/window_impl.h"
#else
#error "Unsupported platform"
#endif

#include <fg/utils/utils.h>
#include <fg/window/window.h>
#include "fg/window/internal.h"

namespace fg::window {

WindowState& GetMutableState(std::weak_ptr<Window> window) {
    return window.lock()->state_;
}

namespace {
// 对于 inline 函数，在链接时会自动只保留一份，其他舍弃
// 此外，inline 函数内的 static 变量是不是只有一份呢？
// 貌似在 c++17 之后才有这个支持，不过我们就是用 17
// 所以这个其实可以写在头文件里面，不过那样写本身也容易混淆
unsigned int GetNewWindowIDImpl() {
    static std::atomic<unsigned int> cur_id{0};
    return cur_id++;
}
}  // namespace

unsigned int Window::GetNewWindowID() { return GetNewWindowIDImpl(); }

Window::Window(const WindowConfig& config) {
    impl_ = std::make_unique<WindowImpl>(config);
    unsigned int window_id = impl_->GetID();
    state_.id = window_id;
    state_.height = config.height;
    state_.width = config.width;
}

void Window::SwapBuffer() const { impl_->SwapBuffer(); }

std::shared_ptr<Window> CreateWindow(const WindowConfig& config) {
    auto window = std::make_shared<Window>(config);
    WindowRegisterInfo reg_info{window->GetID(), window};
    RegisterWindow(reg_info);
    return window;
}

}  // namespace fg::window
