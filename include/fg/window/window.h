#pragma once
#include <atomic>
#include <memory>

#include <fg/macros.h>
#include "fg/window/button.h"

namespace fg::window {

struct FG_API WindowState {
    unsigned int id;
    int height;
    int width;

    float time_stamp = 0.0f;
    bool button_map[256];
    int mouse_x;
    int mouse_y;
};

struct FG_API WindowConfig {
    int width = 800;
    int height = 600;
    const char* name = "";
    bool show_mouse = true;
};

class Window {
    static unsigned int GetNewWindowID();

public:
    struct WindowImpl;

private:
    std::unique_ptr<WindowImpl> impl_;

protected:
    WindowState state_;

public:
    struct WindowImpl;
    Window(const WindowConfig& config);

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&&) = default;
    Window& operator=(Window&&) = default;

    unsigned int GetID() const { return state_.id; }
    const WindowState* GetState() const { return &state_; }
    void SwapBuffer() const;

private:
    friend WindowState& GetMutableState(std::weak_ptr<Window> window);
};

// 可以给个重载版本，不通过 config，直接传参数
std::shared_ptr<Window> FG_API CreateWindow(const WindowConfig& config);

}  // namespace fg::window
