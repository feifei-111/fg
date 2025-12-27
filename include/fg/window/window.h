#pragma once
#include <atomic>
#include <memory>

#include <fg/macros.h>
#include "fg/window/button.h"

namespace fg::window {

struct WindowState {
    unsigned int id;
    int height;
    int width;

    float time_stamp = 0.0f;
    bool button_map[256];
    int mouse_x;
    int mouse_y;
};

struct WindowConfig {
    int width = 800;
    int height = 600;
    const char* name = "";
    bool show_mouse = true;
};

class WindowBase {
    static unsigned int GetNewWindowID();

protected:
    WindowState state_;

public:
    WindowBase() { state_.id = GetNewWindowID(); };

    WindowBase(const WindowBase&) = delete;
    WindowBase& operator=(const WindowBase&) = delete;

    WindowBase(WindowBase&&) = default;
    WindowBase& operator=(WindowBase&&) = default;

    unsigned int GetID() const { return state_.id; }
    const WindowState* GetState() const { return &state_; }
    virtual void SwapBuffer() const = 0;

private:
    friend WindowState* GetMutableState(WindowBase*);
};

template <typename WindowImpl>
class Window : public WindowBase {
    WindowImpl impl_;

public:
    Window(const WindowConfig& config) : impl_(config, this) {}
    void SwapBuffer() const override { impl_.SwapBuffer(); }
};

// 可以给个重载版本，不通过 config，直接传参数
std::shared_ptr<WindowBase> CreateWindow(const WindowConfig& config);

}  // namespace fg::window
