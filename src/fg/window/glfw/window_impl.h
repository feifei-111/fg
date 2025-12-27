#pragma once
#include <GLFW/glfw3.h>
#include <fg/window/window.h>

namespace fg::window::glfw {

class GLFWWindowImpl {
    unsigned int window_id_;
    GLFWwindow* window_;

public:
    GLFWWindowImpl(const WindowConfig& config, WindowBase* base);
    void SwapBuffer() const;
    unsigned int GetID() const;
    ~GLFWWindowImpl();
};

}  // namespace fg::window::glfw
