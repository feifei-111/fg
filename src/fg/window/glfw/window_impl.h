#pragma once
#include <GLFW/glfw3.h>
#include <fg/window/window.h>

namespace fg::window {

class Window::WindowImpl {
    unsigned int window_id_;
    GLFWwindow* window_;

public:
    WindowImpl(const WindowConfig& config);
    void SwapBuffer() const { glfwSwapBuffers(window_); }
    unsigned int GetID() const { return window_id_; }
    ~WindowImpl();
};

}  // namespace fg::window
