/*
    this window creation code with glfw is referred to https://learnopengl.com/
*/
#include <glad/gl.h>

#include <fg/window/event.h>
#include "fg/utils/log.h"
#include "fg/window/glfw/window_impl.h"
#include "fg/window/internal.h"

namespace fg::window {

namespace fg_event = fg::window::event;
using Button = fg::window::Button;
using ButtonMove = fg::window::ButtonMove;

namespace {
void FetchEvent();
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
void MouseButtonCallback(GLFWwindow* window,
                         int glfw_button,
                         int glfw_action,
                         int mods);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void KeyBoardCallback(
  GLFWwindow* window, int key, int scancode, int action, int mods);
}  // namespace

Window::WindowImpl::WindowImpl(const WindowConfig& config) {
    window_id_ = Window::GetNewWindowID();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window_ =
      glfwCreateWindow(config.width, config.height, config.name, NULL, NULL);

    // 这个 user data 主要是需要传入自己的 window id，这样生成的 event 才能对应
    glfwSetWindowUserPointer(window_, reinterpret_cast<void*>(this));

    if (window_ == NULL) {
        VLOG(0) << "glfw create window failed";
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window_);
    glfwSetFramebufferSizeCallback(window_, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window_, MouseMoveCallback);
    glfwSetMouseButtonCallback(window_, MouseButtonCallback);
    glfwSetScrollCallback(window_, ScrollCallback);
    glfwSetKeyCallback(window_, KeyBoardCallback);

    // not show mouse
    if (!config.show_mouse) {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    CHECK(gladLoaderLoadGL()) << "glad init failed";

    event::SetFetchEventHook(FetchEvent);

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
}

Window::WindowImpl::~WindowImpl() {
    glfwTerminate();
    UnregisterWindow(window_id_);
}

namespace {
void FetchEvent() { glfwPollEvents(); }

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto window_impl =
      reinterpret_cast<Window::WindowImpl*>(glfwGetWindowUserPointer(window));
    fg_event::EmplaceEvent<fg_event::PaintEvent>(
      window_impl->GetID(), width, height);
}

void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    auto window_impl =
      reinterpret_cast<Window::WindowImpl*>(glfwGetWindowUserPointer(window));
    // win32 是 int
    // 这里之所以是 double，是为了适配 DPI 倍率
    // 比如说4k屏幕当做 1080p 使用时，逻辑像素坐标会有小数
    // 这里咱们先不管 DPI 啥的，直接 cast 没啥问题
    fg_event::EmplaceEvent<fg_event::MouseMoveEvent>(
      window_impl->GetID(), static_cast<int>(xpos), static_cast<int>(ypos));
}

void MouseButtonCallback(GLFWwindow* window,
                         int glfw_button,
                         int glfw_action,
                         int mods) {
    auto window_impl =
      reinterpret_cast<Window::WindowImpl*>(glfwGetWindowUserPointer(window));
    Button button = Button::UNKNOWN;
    ButtonMove move = ButtonMove::DOWN;
    // glfw 没有 coord
    switch (glfw_button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            button = Button::MS_LBUTTON;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            button = Button::MS_RBUTTON;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            button = Button::MS_MBUTTON;
            break;
        case GLFW_MOUSE_BUTTON_4:
            button = Button::MS_XBUTTON1;
            break;
        case GLFW_MOUSE_BUTTON_5:
            button = Button::MS_XBUTTON2;
            break;
        default:
            CHECK(false) << "Unexcepted mouse button for glfw: " << glfw_button;
    }
    switch (glfw_action) {
        case GLFW_PRESS:
            move = ButtonMove::DOWN;
            break;
        case GLFW_RELEASE:
            move = ButtonMove::UP;
            break;
        default:
            CHECK(false) << "Unexcepted mouse button move for glfw: "
                         << glfw_action;
    }
    fg_event::EmplaceEvent<fg_event::MouseClickEvent>(
      window_impl->GetID(), button, move, 0, 0);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto window_impl =
      reinterpret_cast<Window::WindowImpl*>(glfwGetWindowUserPointer(window));
    // windows 只有单向的，我们就拿单向的得了
    // glfw 没有 coord
    fg_event::EmplaceEvent<fg_event::MouseWheelEvent>(
      window_impl->GetID(), static_cast<int>(yoffset), 0, 0);
}

void KeyBoardCallback(
  GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto window_impl =
      reinterpret_cast<Window::WindowImpl*>(glfwGetWindowUserPointer(window));
    Button button = Button::UNKNOWN;
    ButtonMove move = ButtonMove::DOWN;
    switch (action) {
        case GLFW_PRESS:
        case GLFW_REPEAT:  // 长按连发，就是 DOWN，需要自己判断是否长按
            move = ButtonMove::DOWN;
            break;
        case GLFW_RELEASE:
            move = ButtonMove::UP;
            break;
        default:
            CHECK(false) << "Unexcepted keyboard button move for glfw: "
                         << action;
    }

    switch (key) {
        // F1-F12
        case GLFW_KEY_F1:
            button = Button::F1;
            break;
        case GLFW_KEY_F2:
            button = Button::F2;
            break;
        case GLFW_KEY_F3:
            button = Button::F3;
            break;
        case GLFW_KEY_F4:
            button = Button::F4;
            break;
        case GLFW_KEY_F5:
            button = Button::F5;
            break;
        case GLFW_KEY_F6:
            button = Button::F6;
            break;
        case GLFW_KEY_F7:
            button = Button::F7;
            break;
        case GLFW_KEY_F8:
            button = Button::F8;
            break;
        case GLFW_KEY_F9:
            button = Button::F9;
            break;
        case GLFW_KEY_F10:
            button = Button::F10;
            break;
        case GLFW_KEY_F11:
            button = Button::F11;
            break;
        case GLFW_KEY_F12:
            button = Button::F12;
            break;

        // 主键盘数字 0-9
        case GLFW_KEY_0:
            button = Button::NUM_0;
            break;
        case GLFW_KEY_1:
            button = Button::NUM_1;
            break;
        case GLFW_KEY_2:
            button = Button::NUM_2;
            break;
        case GLFW_KEY_3:
            button = Button::NUM_3;
            break;
        case GLFW_KEY_4:
            button = Button::NUM_4;
            break;
        case GLFW_KEY_5:
            button = Button::NUM_5;
            break;
        case GLFW_KEY_6:
            button = Button::NUM_6;
            break;
        case GLFW_KEY_7:
            button = Button::NUM_7;
            break;
        case GLFW_KEY_8:
            button = Button::NUM_8;
            break;
        case GLFW_KEY_9:
            button = Button::NUM_9;
            break;

        // 字母 A-Z
        case GLFW_KEY_A:
            button = Button::LETTER_A;
            break;
        case GLFW_KEY_B:
            button = Button::LETTER_B;
            break;
        case GLFW_KEY_C:
            button = Button::LETTER_C;
            break;
        case GLFW_KEY_D:
            button = Button::LETTER_D;
            break;
        case GLFW_KEY_E:
            button = Button::LETTER_E;
            break;
        case GLFW_KEY_F:
            button = Button::LETTER_F;
            break;
        case GLFW_KEY_G:
            button = Button::LETTER_G;
            break;
        case GLFW_KEY_H:
            button = Button::LETTER_H;
            break;
        case GLFW_KEY_I:
            button = Button::LETTER_I;
            break;
        case GLFW_KEY_J:
            button = Button::LETTER_J;
            break;
        case GLFW_KEY_K:
            button = Button::LETTER_K;
            break;
        case GLFW_KEY_L:
            button = Button::LETTER_L;
            break;
        case GLFW_KEY_M:
            button = Button::LETTER_M;
            break;
        case GLFW_KEY_N:
            button = Button::LETTER_N;
            break;
        case GLFW_KEY_O:
            button = Button::LETTER_O;
            break;
        case GLFW_KEY_P:
            button = Button::LETTER_P;
            break;
        case GLFW_KEY_Q:
            button = Button::LETTER_Q;
            break;
        case GLFW_KEY_R:
            button = Button::LETTER_R;
            break;
        case GLFW_KEY_S:
            button = Button::LETTER_S;
            break;
        case GLFW_KEY_T:
            button = Button::LETTER_T;
            break;
        case GLFW_KEY_U:
            button = Button::LETTER_U;
            break;
        case GLFW_KEY_V:
            button = Button::LETTER_V;
            break;
        case GLFW_KEY_W:
            button = Button::LETTER_W;
            break;
        case GLFW_KEY_X:
            button = Button::LETTER_X;
            break;
        case GLFW_KEY_Y:
            button = Button::LETTER_Y;
            break;
        case GLFW_KEY_Z:
            button = Button::LETTER_Z;
            break;

        // 控制键
        case GLFW_KEY_BACKSPACE:
            button = Button::BACK;
            break;
        case GLFW_KEY_ENTER:
            button = Button::RETURN;
            break;
        case GLFW_KEY_ESCAPE:
            button = Button::ESC;
            break;

        // 符号键
        case GLFW_KEY_GRAVE_ACCENT:
            button = Button::BACK_QUOTE;  // 反引号
            break;
        case GLFW_KEY_MINUS:
            button = Button::MINUS;  // 减号
            break;
        case GLFW_KEY_EQUAL:
            button = Button::EQUAL;  // 等号
            break;
        case GLFW_KEY_TAB:
            button = Button::TAB;
            break;
        case GLFW_KEY_LEFT_BRACKET:
            button = Button::L_SQUARE_BRACKET;  // 左方括号
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            button = Button::R_SQUARE_BRACKET;  // 右方括号
            break;
        case GLFW_KEY_BACKSLASH:
            button = Button::BACKSLASH;  // 反斜杠
            break;
        case GLFW_KEY_SEMICOLON:
            button = Button::SEMICOLON;  // 分号
            break;
        case GLFW_KEY_APOSTROPHE:
            button = Button::QUOTE;  // 单引号
            break;
        case GLFW_KEY_COMMA:
            button = Button::COMMA;  // 逗号
            break;
        case GLFW_KEY_PERIOD:
            button = Button::PERIOD;  // 句号
            break;
        case GLFW_KEY_SLASH:
            button = Button::SLASH;  // 斜杠
            break;
        case GLFW_KEY_SPACE:
            button = Button::SPACE;
            break;

        // 组合键
        case GLFW_KEY_CAPS_LOCK:
            button = Button::CAPITAL;
            break;
        case GLFW_KEY_LEFT_SHIFT:
            button = Button::LSHIFT;
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            button = Button::RSHIFT;
            break;
        case GLFW_KEY_LEFT_CONTROL:
            button = Button::LCONTROL;
            break;
        case GLFW_KEY_RIGHT_CONTROL:
            button = Button::RCONTROL;
            break;
        case GLFW_KEY_LEFT_ALT:
            button = Button::LMENU;  // Alt
            break;
        case GLFW_KEY_RIGHT_ALT:
            button = Button::RMENU;
            break;
        case GLFW_KEY_LEFT_SUPER:
            button = Button::LWIN;  // Windows/Command
            break;
        case GLFW_KEY_RIGHT_SUPER:
            button = Button::RWIN;
            break;

        // 方向键
        case GLFW_KEY_LEFT:
            button = Button::LEFT;
            break;
        case GLFW_KEY_UP:
            button = Button::UP;
            break;
        case GLFW_KEY_RIGHT:
            button = Button::RIGHT;
            break;
        case GLFW_KEY_DOWN:
            button = Button::DOWN;
            break;

        // 功能键
        case GLFW_KEY_HOME:
            button = Button::HOME;
            break;
        case GLFW_KEY_END:
            button = Button::END;
            break;
        case GLFW_KEY_PAGE_UP:
            button = Button::PRIOR;
            break;
        case GLFW_KEY_PAGE_DOWN:
            button = Button::NEXT;
            break;
        case GLFW_KEY_INSERT:
            button = Button::INSERT;
            break;
        case GLFW_KEY_DELETE:
            button = Button::DEL;
            break;
        case GLFW_KEY_PRINT_SCREEN:
            button = Button::SCREEN_SHOT;
            break;
        case GLFW_KEY_SCROLL_LOCK:
            button = Button::SCROLL;
            break;
        case GLFW_KEY_PAUSE:
            button = Button::PAUSE;
            break;

        // 未匹配的按键
        default:
            button = Button::UNKNOWN;
    }

    fg_event::EmplaceEvent<fg_event::KeyBoardEvent>(
      window_impl->GetID(), button, move);
}
}  // namespace

}  // namespace fg::window
