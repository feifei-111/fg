/*
    this window creation code with glfw is referred to https://learnopengl.com/
*/
#include <glad/gl.h>

#include <fg/window/event.h>
#include "fg/utils/log.h"
#include "fg/window/event_internal.h"
#include "fg/window/glfw/window_impl.h"
#include "fg/window/registry.h"
#include "fg/window/window_internal.h"

namespace fg::window::glfw {

namespace fg_event = fg::window::event;
using Event = fg_event::Event;
using Button = fg::window::Button;
using ButtonMove = fg::window::ButtonMove;

namespace {
void FetchEvent();
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void KeyBoardCallback(
  GLFWwindow* window, int key, int scancode, int action, int mods);
}  // namespace

GLFWWindowImpl::GLFWWindowImpl(const WindowConfig& config, WindowBase* base) {
    WindowState* state = GetMutableState(base);
    window_id_ = state->id;

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
    if (!gladLoaderLoadGL()) {
        VLOG(0) << "glad init failed";
        exit(1);
    }

    WindowRegisterInfo reg_info;
    reg_info.id = window_id_;
    reg_info.window = base;
    RegisterWindow(reg_info);

    event::SetFetchEvent(FetchEvent);

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
}

GLFWWindowImpl::~GLFWWindowImpl() {
    glfwTerminate();
    UnregisterWindow(window_id_);
}

void GLFWWindowImpl::SwapBuffer() const { glfwSwapBuffers(window_); }

unsigned int GLFWWindowImpl::GetID() const { return window_id_; }

namespace {
void FetchEvent() { glfwPollEvents(); }

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto window_impl =
      reinterpret_cast<GLFWWindowImpl*>(glfwGetWindowUserPointer(window));
    Event* next_event = fg_event::PrepareNextEvent(window_impl->GetID());
    next_event->data.emplace<fg_event::PaintEvent>();
    auto& event_data = std::get<fg_event::PaintEvent>(next_event->data);
    event_data.width = width;
    event_data.height = height;
    fg_event::PrepareEventDone();
}

void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    auto window_impl =
      reinterpret_cast<GLFWWindowImpl*>(glfwGetWindowUserPointer(window));
    Event* next_event = fg_event::PrepareNextEvent(window_impl->GetID());
    next_event->data.emplace<fg_event::MouseMoveEvent>();
    auto& event_data = std::get<fg_event::MouseMoveEvent>(next_event->data);
    // win32 是 int
    // 这里之所以是 double，是为了适配 DPI 倍率
    // 比如说4k屏幕当做 1080p 使用时，逻辑像素坐标会有小数
    // 这里咱们先不管 DPI 啥的，直接 cast 没啥问题
    event_data.x = static_cast<int>(xpos);
    event_data.y = static_cast<int>(ypos);
    fg_event::PrepareEventDone();
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto window_impl =
      reinterpret_cast<GLFWWindowImpl*>(glfwGetWindowUserPointer(window));
    Event* next_event = fg_event::PrepareNextEvent(window_impl->GetID());
    next_event->data.emplace<fg_event::MouseClickEvent>();
    auto& event_data = std::get<fg_event::MouseClickEvent>(next_event->data);
    // glfw 没有 coord
    event_data.x = static_cast<int>(0);
    event_data.y = static_cast<int>(0);
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            event_data.button = Button::MS_LBUTTON;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            event_data.button = Button::MS_RBUTTON;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            event_data.button = Button::MS_MBUTTON;
            break;
        case GLFW_MOUSE_BUTTON_4:
            event_data.button = Button::MS_XBUTTON1;
            break;
        case GLFW_MOUSE_BUTTON_5:
            event_data.button = Button::MS_XBUTTON2;
            break;
        default:
            CHECK(false) << "Unexcepted mouse button for glfw: " << button;
    }
    switch (action) {
        case GLFW_PRESS:
            event_data.move = ButtonMove::DOWN;
            break;
        case GLFW_RELEASE:
            event_data.move = ButtonMove::UP;
            break;
        default:
            CHECK(false) << "Unexcepted mouse button move for glfw: " << action;
    }
    fg_event::PrepareEventDone();
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto window_impl =
      reinterpret_cast<GLFWWindowImpl*>(glfwGetWindowUserPointer(window));
    Event* next_event = fg_event::PrepareNextEvent(window_impl->GetID());
    next_event->data.emplace<fg_event::MouseWheelEvent>();
    auto& event_data = std::get<fg_event::MouseWheelEvent>(next_event->data);
    // windows 只有单向的，我们就拿单向的得了
    event_data.wheel_delta = static_cast<int>(yoffset);
    // glfw 没有 coord
    event_data.x = 0;
    event_data.y = 0;
    fg_event::PrepareEventDone();
}

void KeyBoardCallback(
  GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto window_impl =
      reinterpret_cast<GLFWWindowImpl*>(glfwGetWindowUserPointer(window));
    Event* next_event = fg_event::PrepareNextEvent(window_impl->GetID());
    next_event->data.emplace<fg_event::KeyBoardEvent>();
    auto& event_data = std::get<fg_event::KeyBoardEvent>(next_event->data);
    switch (action) {
        case GLFW_PRESS:
        case GLFW_REPEAT:  // 长按连发，就是 DOWN，需要自己判断是否长按
            event_data.move = ButtonMove::DOWN;
            break;
        case GLFW_RELEASE:
            event_data.move = ButtonMove::UP;
            break;
        default:
            CHECK(false) << "Unexcepted keyboard button move for glfw: "
                         << action;
    }

    switch (key) {
        // F1-F12
        case GLFW_KEY_F1:
            event_data.button = Button::F1;
            break;
        case GLFW_KEY_F2:
            event_data.button = Button::F2;
            break;
        case GLFW_KEY_F3:
            event_data.button = Button::F3;
            break;
        case GLFW_KEY_F4:
            event_data.button = Button::F4;
            break;
        case GLFW_KEY_F5:
            event_data.button = Button::F5;
            break;
        case GLFW_KEY_F6:
            event_data.button = Button::F6;
            break;
        case GLFW_KEY_F7:
            event_data.button = Button::F7;
            break;
        case GLFW_KEY_F8:
            event_data.button = Button::F8;
            break;
        case GLFW_KEY_F9:
            event_data.button = Button::F9;
            break;
        case GLFW_KEY_F10:
            event_data.button = Button::F10;
            break;
        case GLFW_KEY_F11:
            event_data.button = Button::F11;
            break;
        case GLFW_KEY_F12:
            event_data.button = Button::F12;
            break;

        // 主键盘数字 0-9
        case GLFW_KEY_0:
            event_data.button = Button::NUM_0;
            break;
        case GLFW_KEY_1:
            event_data.button = Button::NUM_1;
            break;
        case GLFW_KEY_2:
            event_data.button = Button::NUM_2;
            break;
        case GLFW_KEY_3:
            event_data.button = Button::NUM_3;
            break;
        case GLFW_KEY_4:
            event_data.button = Button::NUM_4;
            break;
        case GLFW_KEY_5:
            event_data.button = Button::NUM_5;
            break;
        case GLFW_KEY_6:
            event_data.button = Button::NUM_6;
            break;
        case GLFW_KEY_7:
            event_data.button = Button::NUM_7;
            break;
        case GLFW_KEY_8:
            event_data.button = Button::NUM_8;
            break;
        case GLFW_KEY_9:
            event_data.button = Button::NUM_9;
            break;

        // 字母 A-Z
        case GLFW_KEY_A:
            event_data.button = Button::LETTER_A;
            break;
        case GLFW_KEY_B:
            event_data.button = Button::LETTER_B;
            break;
        case GLFW_KEY_C:
            event_data.button = Button::LETTER_C;
            break;
        case GLFW_KEY_D:
            event_data.button = Button::LETTER_D;
            break;
        case GLFW_KEY_E:
            event_data.button = Button::LETTER_E;
            break;
        case GLFW_KEY_F:
            event_data.button = Button::LETTER_F;
            break;
        case GLFW_KEY_G:
            event_data.button = Button::LETTER_G;
            break;
        case GLFW_KEY_H:
            event_data.button = Button::LETTER_H;
            break;
        case GLFW_KEY_I:
            event_data.button = Button::LETTER_I;
            break;
        case GLFW_KEY_J:
            event_data.button = Button::LETTER_J;
            break;
        case GLFW_KEY_K:
            event_data.button = Button::LETTER_K;
            break;
        case GLFW_KEY_L:
            event_data.button = Button::LETTER_L;
            break;
        case GLFW_KEY_M:
            event_data.button = Button::LETTER_M;
            break;
        case GLFW_KEY_N:
            event_data.button = Button::LETTER_N;
            break;
        case GLFW_KEY_O:
            event_data.button = Button::LETTER_O;
            break;
        case GLFW_KEY_P:
            event_data.button = Button::LETTER_P;
            break;
        case GLFW_KEY_Q:
            event_data.button = Button::LETTER_Q;
            break;
        case GLFW_KEY_R:
            event_data.button = Button::LETTER_R;
            break;
        case GLFW_KEY_S:
            event_data.button = Button::LETTER_S;
            break;
        case GLFW_KEY_T:
            event_data.button = Button::LETTER_T;
            break;
        case GLFW_KEY_U:
            event_data.button = Button::LETTER_U;
            break;
        case GLFW_KEY_V:
            event_data.button = Button::LETTER_V;
            break;
        case GLFW_KEY_W:
            event_data.button = Button::LETTER_W;
            break;
        case GLFW_KEY_X:
            event_data.button = Button::LETTER_X;
            break;
        case GLFW_KEY_Y:
            event_data.button = Button::LETTER_Y;
            break;
        case GLFW_KEY_Z:
            event_data.button = Button::LETTER_Z;
            break;

        // 控制键
        case GLFW_KEY_BACKSPACE:
            event_data.button = Button::BACK;
            break;
        case GLFW_KEY_ENTER:
            event_data.button = Button::RETURN;
            break;
        case GLFW_KEY_ESCAPE:
            event_data.button = Button::ESC;
            break;

        // 符号键
        case GLFW_KEY_GRAVE_ACCENT:
            event_data.button = Button::BACK_QUOTE;  // 反引号
            break;
        case GLFW_KEY_MINUS:
            event_data.button = Button::MINUS;  // 减号
            break;
        case GLFW_KEY_EQUAL:
            event_data.button = Button::EQUAL;  // 等号
            break;
        case GLFW_KEY_TAB:
            event_data.button = Button::TAB;
            break;
        case GLFW_KEY_LEFT_BRACKET:
            event_data.button = Button::L_SQUARE_BRACKET;  // 左方括号
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            event_data.button = Button::R_SQUARE_BRACKET;  // 右方括号
            break;
        case GLFW_KEY_BACKSLASH:
            event_data.button = Button::BACKSLASH;  // 反斜杠
            break;
        case GLFW_KEY_SEMICOLON:
            event_data.button = Button::SEMICOLON;  // 分号
            break;
        case GLFW_KEY_APOSTROPHE:
            event_data.button = Button::QUOTE;  // 单引号
            break;
        case GLFW_KEY_COMMA:
            event_data.button = Button::COMMA;  // 逗号
            break;
        case GLFW_KEY_PERIOD:
            event_data.button = Button::PERIOD;  // 句号
            break;
        case GLFW_KEY_SLASH:
            event_data.button = Button::SLASH;  // 斜杠
            break;
        case GLFW_KEY_SPACE:
            event_data.button = Button::SPACE;
            break;

        // 组合键
        case GLFW_KEY_CAPS_LOCK:
            event_data.button = Button::CAPITAL;
            break;
        case GLFW_KEY_LEFT_SHIFT:
            event_data.button = Button::LSHIFT;
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            event_data.button = Button::RSHIFT;
            break;
        case GLFW_KEY_LEFT_CONTROL:
            event_data.button = Button::LCONTROL;
            break;
        case GLFW_KEY_RIGHT_CONTROL:
            event_data.button = Button::RCONTROL;
            break;
        case GLFW_KEY_LEFT_ALT:
            event_data.button = Button::LMENU;  // Alt
            break;
        case GLFW_KEY_RIGHT_ALT:
            event_data.button = Button::RMENU;
            break;
        case GLFW_KEY_LEFT_SUPER:
            event_data.button = Button::LWIN;  // Windows/Command
            break;
        case GLFW_KEY_RIGHT_SUPER:
            event_data.button = Button::RWIN;
            break;

        // 方向键
        case GLFW_KEY_LEFT:
            event_data.button = Button::LEFT;
            break;
        case GLFW_KEY_UP:
            event_data.button = Button::UP;
            break;
        case GLFW_KEY_RIGHT:
            event_data.button = Button::RIGHT;
            break;
        case GLFW_KEY_DOWN:
            event_data.button = Button::DOWN;
            break;

        // 功能键
        case GLFW_KEY_HOME:
            event_data.button = Button::HOME;
            break;
        case GLFW_KEY_END:
            event_data.button = Button::END;
            break;
        case GLFW_KEY_PAGE_UP:
            event_data.button = Button::PRIOR;
            break;
        case GLFW_KEY_PAGE_DOWN:
            event_data.button = Button::NEXT;
            break;
        case GLFW_KEY_INSERT:
            event_data.button = Button::INSERT;
            break;
        case GLFW_KEY_DELETE:
            event_data.button = Button::DEL;
            break;
        case GLFW_KEY_PRINT_SCREEN:
            event_data.button = Button::SCREEN_SHOT;
            break;
        case GLFW_KEY_SCROLL_LOCK:
            event_data.button = Button::SCROLL;
            break;
        case GLFW_KEY_PAUSE:
            event_data.button = Button::PAUSE;
            break;

        // 未匹配的按键
        default:
            event_data.button = Button::UNKNOWN;
    }

    fg_event::PrepareEventDone();
}
}  // namespace

}  // namespace fg::window::glfw
