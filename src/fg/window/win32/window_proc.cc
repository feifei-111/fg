#include <Windowsx.h>

#include <glad/gl.h>

#include <fg/window/event.h>
#include "fg/utils/log.h"
#include "fg/window/event_internal.h"
#include "fg/window/win32/window_impl.h"
#include "fg/window/win32/window_proc.h"

namespace fg::window::win32 {

namespace fg_event = fg::window::event;
using Event = fg_event::Event;
using Button = fg::window::Button;
using ButtonMove = fg::window::ButtonMove;

namespace {

Button GetButtonFromVK(WPARAM wparam);
}  // namespace

LRESULT CALLBACK WindowProc(HWND hwnd,
                            UINT uMsg,
                            WPARAM wParam,
                            LPARAM lParam) {
    VLOG(9) << "WindowProc GetMsg: " << uMsg << ", " << wParam << ", "
            << lParam;

    Win32WindowImpl* window_impl = reinterpret_cast<Win32WindowImpl*>(
      GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    // GWLP_USERDATA 在 WM_NCCREATE 之前还没设置，此时为空
    if (!window_impl || !window_impl->IsReady()) {
        switch (uMsg) {
            case WM_NCCREATE: {
                auto user_data =
                  (LONG_PTR)(((CREATESTRUCT*)lParam)->lpCreateParams);
                CHECK(user_data)
                  << "lpParams(user data) not exist, Win32 window init failed";
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, user_data);
                return DefWindowProcW(hwnd, uMsg, wParam, lParam);
            }
            default:
                return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }
    }

    CHECK(window_impl->GetHWND() != hwnd)
      << "Win32Data and hwnd is not correlated!";

    Event* next_event = fg_event::PrepareNextEvent(window_impl->GetID());

    switch (uMsg) {
        case WM_DESTROY:
            ReleaseDC(hwnd, window_impl->GetHDC());
            PostQuitMessage(0);
            next_event->data.emplace<fg_event::ExitEvent>();
            fg_event::PrepareEventDone();
            return 0;

        case WM_PAINT:
            // 不考虑局部重绘，GetWindowRect 获取整个 window size
            RECT window_rect;
            GetWindowRect(hwnd, &window_rect);
            next_event->data.emplace<fg_event::PaintEvent>(
              window_rect.bottom - window_rect.top,  // height
              window_rect.right - window_rect.left   // width
            );
            fg_event::PrepareEventDone();
            // 我们并不自己去重新渲染，应该是游戏逻辑自己去做这件事
            // glViewport(0, 0, state->width, state->height);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_MOUSEMOVE:
            next_event->data.emplace<fg_event::MouseMoveEvent>(
              GET_X_LPARAM(lParam),  // x
              GET_Y_LPARAM(lParam)   // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_MOUSEWHEEL:
            next_event->data.emplace<fg_event::MouseWheelEvent>(
              GET_WHEEL_DELTA_WPARAM(wParam),  // wheel delta
              GET_X_LPARAM(lParam),            // x
              GET_Y_LPARAM(lParam)             // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_LBUTTONDOWN:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              Button::MS_LBUTTON,    // button
              ButtonMove::DOWN,      // move
              GET_X_LPARAM(lParam),  // x
              GET_Y_LPARAM(lParam)   // y
            );
            fg_event::PrepareEventDone();
            SetCapture(hwnd);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_LBUTTONUP:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              Button::MS_LBUTTON,    // button
              ButtonMove::UP,        // move
              GET_X_LPARAM(lParam),  // x
              GET_Y_LPARAM(lParam)   // y
            );
            fg_event::PrepareEventDone();
            ReleaseCapture();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        // double click 不会影响单独的 down up msg，会在第二次的 down up
        // 之间插入一个额外的 msg
        case WM_LBUTTONDBLCLK:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              Button::MS_LBUTTON,    // button
              ButtonMove::DCLICK,    // move
              GET_X_LPARAM(lParam),  // x
              GET_Y_LPARAM(lParam)   // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_RBUTTONDOWN:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              Button::MS_RBUTTON,    // button
              ButtonMove::DOWN,      // move
              GET_X_LPARAM(lParam),  // x
              GET_Y_LPARAM(lParam)   // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_RBUTTONUP:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              Button::MS_RBUTTON,    // button
              ButtonMove::UP,        // move
              GET_X_LPARAM(lParam),  // x
              GET_Y_LPARAM(lParam)   // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_RBUTTONDBLCLK:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              Button::MS_RBUTTON,    // button
              ButtonMove::DCLICK,    // move
              GET_X_LPARAM(lParam),  // x
              GET_Y_LPARAM(lParam)   // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_MBUTTONDOWN:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              Button::MS_MBUTTON,    // button
              ButtonMove::DOWN,      // move
              GET_X_LPARAM(lParam),  // x
              GET_Y_LPARAM(lParam)   // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_MBUTTONUP:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              Button::MS_MBUTTON,    // button
              ButtonMove::UP,        // move
              GET_X_LPARAM(lParam),  // x
              GET_Y_LPARAM(lParam)   // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_MBUTTONDBLCLK:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              Button::MS_MBUTTON,    // button
              ButtonMove::DCLICK,    // move
              GET_X_LPARAM(lParam),  // x
              GET_Y_LPARAM(lParam)   // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_XBUTTONDOWN:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                ? Button::MS_XBUTTON1
                : Button::MS_XBUTTON2,  // button
              ButtonMove::DOWN,         // move
              GET_X_LPARAM(lParam),     // x
              GET_Y_LPARAM(lParam)      // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_XBUTTONUP:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                ? Button::MS_XBUTTON1
                : Button::MS_XBUTTON2,  // button
              ButtonMove::UP,           // move
              GET_X_LPARAM(lParam),     // x
              GET_Y_LPARAM(lParam)      // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_XBUTTONDBLCLK:
            next_event->data.emplace<fg_event::MouseClickEvent>(
              (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                ? Button::MS_XBUTTON1
                : Button::MS_XBUTTON2,  // button
              ButtonMove::DCLICK,       // move
              GET_X_LPARAM(lParam),     // x
              GET_Y_LPARAM(lParam)      // y
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_KEYDOWN:
            next_event->data.emplace<fg_event::KeyBoardEvent>(
              GetButtonFromVK(wParam),  // button
              ButtonMove::DOWN          // move
            );
            // 感觉 repeat 也没什么用，还是不记录了
            // repeat = LOWORD(lParam);
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_KEYUP:
            next_event->data.emplace<fg_event::KeyBoardEvent>(
              GetButtonFromVK(wParam),  // button
              ButtonMove::UP            // move
            );
            fg_event::PrepareEventDone();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_CHAR:  // 支持游戏内文本输入需要用，暂时不搞
        case WM_SYSKEYDOWN:  // alt+F4，alt+tab
                             // 之类的系统按键重定义行为需要用，不搞
        case WM_SYSKEYUP:

        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}
namespace {

Button GetButtonFromVK(WPARAM wparam) {
    switch (wparam) {
        case VK_F1:
            return Button::F1;
        case VK_F2:
            return Button::F2;
        case VK_F3:
            return Button::F3;
        case VK_F4:
            return Button::F4;
        case VK_F5:
            return Button::F5;
        case VK_F6:
            return Button::F6;
        case VK_F7:
            return Button::F7;
        case VK_F8:
            return Button::F8;
        case VK_F9:
            return Button::F9;
        case VK_F10:
            return Button::F10;
        case VK_F11:
            return Button::F11;
        case VK_F12:
            return Button::F12;

        // 主要数字字母符号直接对应 ascii code
        case '0':
            return Button::NUM_0;
        case '1':
            return Button::NUM_1;
        case '2':
            return Button::NUM_2;
        case '3':
            return Button::NUM_3;
        case '4':
            return Button::NUM_4;
        case '5':
            return Button::NUM_5;
        case '6':
            return Button::NUM_6;
        case '7':
            return Button::NUM_7;
        case '8':
            return Button::NUM_8;
        case '9':
            return Button::NUM_9;

        case 'A':
            return Button::LETTER_A;
        case 'B':
            return Button::LETTER_B;
        case 'C':
            return Button::LETTER_C;
        case 'D':
            return Button::LETTER_D;
        case 'E':
            return Button::LETTER_E;
        case 'F':
            return Button::LETTER_F;
        case 'G':
            return Button::LETTER_G;
        case 'H':
            return Button::LETTER_H;
        case 'I':
            return Button::LETTER_I;
        case 'J':
            return Button::LETTER_J;
        case 'K':
            return Button::LETTER_K;
        case 'L':
            return Button::LETTER_L;
        case 'M':
            return Button::LETTER_M;
        case 'N':
            return Button::LETTER_N;
        case 'O':
            return Button::LETTER_O;
        case 'P':
            return Button::LETTER_P;
        case 'Q':
            return Button::LETTER_Q;
        case 'R':
            return Button::LETTER_R;
        case 'S':
            return Button::LETTER_S;
        case 'T':
            return Button::LETTER_T;
        case 'U':
            return Button::LETTER_U;
        case 'V':
            return Button::LETTER_V;
        case 'W':
            return Button::LETTER_W;
        case 'X':
            return Button::LETTER_X;
        case 'Y':
            return Button::LETTER_Y;
        case 'Z':
            return Button::LETTER_Z;

        // 控制
        case VK_BACK:
            return Button::BACK;  // Backspace
        case VK_RETURN:
            return Button::RETURN;
        case VK_ESCAPE:
            return Button::ESC;

        // 符号
        case VK_OEM_3:
            return Button::BACK_QUOTE;  // ` ~
        case VK_OEM_MINUS:
            return Button::MINUS;  // - _
        case VK_OEM_PLUS:
            return Button::EQUAL;  // = +
        case VK_TAB:
            return Button::TAB;  // Tab
        case VK_OEM_4:
            return Button::L_SQUARE_BRACKET;  // [ {
        case VK_OEM_6:
            return Button::R_SQUARE_BRACKET;  // ] }
        case VK_OEM_5:
            return Button::BACKSLASH;  // \ |
        case VK_OEM_1:
            return Button::SEMICOLON;  // ; :
        case VK_OEM_7:
            return Button::QUOTE;  // ' "
        case VK_OEM_COMMA:
            return Button::COMMA;  // , <
        case VK_OEM_PERIOD:
            return Button::PERIOD;  // . >
        case VK_OEM_2:
            return Button::SLASH;  // / ?

        // 组合键
        case VK_CAPITAL:
            return Button::CAPITAL;  // Caps
        case VK_LSHIFT:
            return Button::LSHIFT;
        case VK_RSHIFT:
            return Button::RSHIFT;
        case VK_LCONTROL:
            return Button::LCONTROL;
        case VK_RCONTROL:
            return Button::RCONTROL;
        case VK_LMENU:
            return Button::LMENU;  // alt
        case VK_RMENU:
            return Button::RMENU;
        case VK_LWIN:
            return Button::LWIN;
        case VK_RWIN:
            return Button::RWIN;
        case VK_SPACE:
            return Button::SPACE;

        // 方向键
        case VK_LEFT:
            return Button::LEFT;
        case VK_UP:
            return Button::UP;
        case VK_RIGHT:
            return Button::RIGHT;
        case VK_DOWN:
            return Button::DOWN;

        // 功能键
        case VK_HOME:
            return Button::HOME;
        case VK_END:
            return Button::END;
        case VK_PRIOR:
            return Button::PRIOR;  // PageUp
        case VK_NEXT:
            return Button::NEXT;  // PageDown
        case VK_INSERT:
            return Button::INSERT;
        case VK_DELETE:
            return Button::DEL;
        case VK_SNAPSHOT:
            return Button::SCREEN_SHOT;  // PrintScreen
        case VK_SCROLL:
            return Button::SCROLL;  // ScrollLock
        case VK_PAUSE:
            return Button::PAUSE;  // Pause

        // 小键盘
        case VK_NUMLOCK:
            return Button::NUMLOCK;
        case VK_NUMPAD0:
            return Button::NUMPAD_0;
        case VK_NUMPAD1:
            return Button::NUMPAD_1;
        case VK_NUMPAD2:
            return Button::NUMPAD_2;
        case VK_NUMPAD3:
            return Button::NUMPAD_3;
        case VK_NUMPAD4:
            return Button::NUMPAD_4;
        case VK_NUMPAD5:
            return Button::NUMPAD_5;
        case VK_NUMPAD6:
            return Button::NUMPAD_6;
        case VK_NUMPAD7:
            return Button::NUMPAD_7;
        case VK_NUMPAD8:
            return Button::NUMPAD_8;
        case VK_NUMPAD9:
            return Button::NUMPAD_9;
        case VK_MULTIPLY:
            return Button::MULTIPLY;  // *
        case VK_ADD:
            return Button::ADD;  // +
        case VK_SEPARATOR:
            return Button::SEPARATOR;  // 分隔符
        case VK_SUBTRACT:
            return Button::SUBTRACT;  // -
        case VK_DECIMAL:
            return Button::DECIMAL;  // .
        case VK_DIVIDE:
            return Button::DIVIDE;  // /

        default:
            return Button::UNKNOWN;
    }
}

}  // namespace
}  // namespace fg::window::win32
