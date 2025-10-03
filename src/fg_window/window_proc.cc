#include <iostream>

#include "fg_window/window.h"
#include "fg_window/window_proc.h"
#include "fg_interact/event.h"
#include "fg_utils/utils.h"


namespace fei_window {

using Event = fg_interact::Event;
using EventType = fg_interact::EventType;
using Button = fg_interact::Button;
using ButtonMove = fg_interact::ButtonMove

static inline void PushMouseClickEvent(Button button, ButtonMove move, LPARAM lparam);
static inline Button GetButtonFromParam(LPARAM lparam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

    // std::cout << uMsg << ", " << wParam << ", " <<  lParam << std::endl;

    WindowData* data;
    data = (WindowData*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    // 初始化未完成，至少等到 Window 整个完成再处理 msg（此时 data->window 存在）
    if (!data || !data->window){
        switch (uMsg) {
            case WM_NCCREATE:
                data = (WindowData*)(((CREATESTRUCT*)lParam)->lpCreateParams);
                if (!data){
                    MessageBoxW(NULL, L"window data init error!", L"Error", MB_ICONERROR);
                    exit(1);
                }
                SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)data);
                return DefWindowProcW(hwnd, uMsg, wParam, lParam);
            default:
                return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }
    }

    // 以下，初始化已经完成， data 和 data->window 已经设置好了
    if (data->window->GetHWND() != hwnd){
        MessageBoxW(NULL, L"hwnd not correct!", L"Error", MB_ICONERROR);
    }
    
    switch (uMsg) {
        case WM_DESTROY:
            ReleaseDC(hwnd, data->window->GetHDC());
            PostQuitMessage(0);
            Event event;
            event.type = EventType::kExitEvent;
            PushEvent(&event);
            return 0;

        case WM_PAINT:
            // 不考虑局部重绘，GetWindowRect 获取整个 window size
            RECT window_rect;
            GetWindowRect(hwnd, &window_rect);
            int width = window_rect.right -  window_rect.left;
            int height = window_rect.bottom - window_rect.top;
            data->status.height = height;
            data->status.width = width;
            glViewport(0, 0, width, height);
            Event event;
            event.type = EventType::kPaintEvent;
            PushEvent(&event);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_MOUSEMOVE:
            Event event;
            event.type = EventType::kMouseMoveEvent;
            event.data.mouse_move_event.x = GET_X_LPARAM(lParam);
            event.data.mouse_move_event.y = GET_Y_LPARAM(lParam);
            PushEvent(&event)
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_LBUTTONDOWN:
            PushMouseClickEvent(Button::MS_LButton, ButtonMove::DOWN, lparam);
            SetCapture(hwnd);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_LBUTTONUP:
            PushMouseClickEvent(Button::MS_LButton, ButtonMove::UP, lparam);
            ReleaseCapture();
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        // double click 不会影响单独的 down up msg，会在第二次的 down up 之间插入一个额外的 msg
        case: WM_LBUTTONDBLCLK:
            PushMouseClickEvent(Button::MS_LButton, ButtonMove::DCLICK, lparam);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_RBUTTONDOWN:
            PushMouseClickEvent(Button::MS_RButton, ButtonMove::DOWN, lparam);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_RBUTTONUP:
            PushMouseClickEvent(Button::MS_RButton, ButtonMove::UP, lparam);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case: WM_RBUTTONDBLCLK:
            PushMouseClickEvent(Button::MS_RButton, ButtonMove::DCLICK, lparam);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_MBUTTONDOWN:
            PushMouseClickEvent(Button::MS_MButton, ButtonMove::DOWN, lparam);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_MBUTTONUP:
            PushMouseClickEvent(Button::MS_MButton, ButtonMove::UP, lparam);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_MBUTTONDBLCLK:
            PushMouseClickEvent(Button::MS_MButton, ButtonMove::DCLICK, lparam);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);            

        case WM_XBUTTONDOWN:
            Button button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? Button::MS_XBUTTON1 : Button::MS_XBUTTON2;
            PushMouseClickEvent(button, ButtonMove::DOWN, lparam);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam); 

        case WM_XBUTTONUP:
            Button button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? Button::MS_XBUTTON1 : Button::MS_XBUTTON2;
            PushMouseClickEvent(button, ButtonMove::UP, lparam);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam); 

        case WM_XBUTTONDBLCLK:
            Button button = (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) ? Button::MS_XBUTTON1 : Button::MS_XBUTTON2;
            PushMouseClickEvent(button, ButtonMove::DCLICK, lparam);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam); 

        case WM_MOUSEWHEEL: 
            Event event;
            event.type = EventType::kMouseWheelEvent;
            event.data.mouse_wheel_event.wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam);
            event.data.mouse_wheel_event.x = GET_X_LPARAM(lParam);
            event.data.mouse_wheel_event.y = GET_Y_LPARAM(lParam);
            PushEvent(&event);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_KEYDOWN:
            Event event;
            event.type = EventType::kKeyboardEvent;
            event.data.keyboard_event.button = GetButtonFromParam(lParam);
            event.data.keyboard_event.move = ButtonMove::DOWN;
            // 感觉 repeat 也没什么用，还是不记录了
            // event.data.keyboard_event.repeat = LOWORD(lParam);
            PushEvent(&event);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_KEYUP:
            Event event;
            event.type = EventType::kKeyboardEvent;
            event.data.keyboard_event.button = GetButtonFromParam(lParam);
            event.data.keyboard_event.move = ButtonMove::UP;
            // event.data.keyboard_event.repeat = LOWORD(lParam);
            PushEvent(&event);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);

        case WM_CHAR:   // 支持游戏内文本输入需要用，暂时不搞
        case WM_SYSKEYDOWN: // alt+F4，alt+tab 之类的系统按键重定义行为需要用，不搞
        case WM_SYSKEYUP:

        default:
            Event event;
            event.type = EventType::kCommonEvent;
            event.data.common_event.win_msg = uMsg;
            event.data.common_event.wparam = wParam;
            event.data.common_event.lparam = lParam;
            PushEvent(&event);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam); // 默认消息处理
    }
}


static inline void PushMouseClickEvent(Button button, ButtonMove move, LPARAM lparam){
    Event event;
    event.type = EventType::kMouseClickEvent;
    event.data.mouse_click_event.button = button;
    event.data.mouse_click_event.move = move;
    event.data.mouse_click_event.x = GET_X_LPARAM(lparam);
    event.data.mouse_click_event.y = GET_Y_LPARAM(lparam);
    PushEvent(&event);
}

Button GetButtonFromParam(LPARAM lparam){
    uint16_t scanCode = (lParam >> 16) & 0xff;
    bool extended = (lparam & 0x1000000);

    switch (scan_code) {
        case 0x3B: return Button::F1;
        case 0x3C: return Button::F2;
        case 0x3D: return Button::F3;
        case 0x3E: return Button::F4;
        case 0x3F: return Button::F5;
        case 0x40: return Button::F6;
        case 0x41: return Button::F7;
        case 0x42: return Button::F8;
        case 0x43: return Button::F9;
        case 0x44: return Button::F10;
        case 0x57: return Button::F11;
        case 0x58: return Button::F12;

        case 0x0B: return Button::NUM_0;
        case 0x02: return Button::NUM_1;
        case 0x03: return Button::NUM_2;
        case 0x04: return Button::NUM_3;
        case 0x05: return Button::NUM_4;
        case 0x06: return Button::NUM_5;
        case 0x07: return Button::NUM_6;
        case 0x08: return Button::NUM_7;
        case 0x09: return Button::NUM_8;
        case 0x0A: return Button::NUM_9;

        case 0x1E: return Button::LETTER_A;
        case 0x30: return Button::LETTER_B;
        case 0x2E: return Button::LETTER_C;
        case 0x20: return Button::LETTER_D;
        case 0x12: return Button::LETTER_E;
        case 0x21: return Button::LETTER_F;
        case 0x22: return Button::LETTER_G;
        case 0x23: return Button::LETTER_H;
        case 0x17: return Button::LETTER_I;
        case 0x24: return Button::LETTER_J;
        case 0x25: return Button::LETTER_K;
        case 0x26: return Button::LETTER_L;
        case 0x32: return Button::LETTER_M;
        case 0x31: return Button::LETTER_N;
        case 0x18: return Button::LETTER_O;
        case 0x19: return Button::LETTER_P;
        case 0x10: return Button::LETTER_Q;
        case 0x13: return Button::LETTER_R;
        case 0x1F: return Button::LETTER_S;
        case 0x14: return Button::LETTER_T;
        case 0x16: return Button::LETTER_U;
        case 0x2F: return Button::LETTER_V;
        case 0x11: return Button::LETTER_W;
        case 0x2D: return Button::LETTER_X;
        case 0x15: return Button::LETTER_Y;
        case 0x2C: return Button::LETTER_Z;

        case 0x0E: return Button::BACK;          // Backspace
        case 0x1C: return Button::RETURN;        // Enter
        case 0x01: return Button::ESC;        // Escape

        case 0x29: return Button::BACK_QUOTE;    // `~
        case 0x0C: return Button::MINUS;         // -_
        case 0x0D: return Button::EQUAL;         // =+

        case 0x0F: return Button::TAB;           // Tab
        case 0x1A: return Button::L_SQUARE_BRACKET; // [ {
        case 0x1B: return Button::R_SQUARE_BRACKET; // ] }
        case 0x2B: return Button::BACKSLASH;     // \ |

        case 0x27: return Button::SEMICOLON;     // ; :
        case 0x28: return Button::QUOTE;         // ' "

        case 0x33: return Button::COMMA;         // , <
        case 0x34: return Button::PERIOD;        // . >
        case 0x35: return Button::SLASH;         // / ?

        case 0x39: return Button::SPACE;         // Space

        case 0x3A: return Button::CAPITAL;       // Caps Lock
        case 0x2A: return Button::LSHIFT;        // Left Shift
        case 0x36: return Button::RSHIFT;        // Right Shift
        case 0x1D: return (extended ? Button::RCONTROL : Button::LCONTROL); // Ctrl（扩展码为右Ctrl）
        case 0x38: return (extended ? Button::RMENU : Button::LMENU);       // Alt（扩展码为右Alt）
        case 0x5B: return Button::LWIN;          // Left Win
        case 0x5C: return Button::RWIN;          // Right Win

        // 方向键（依赖扩展码标识）
        case 0x4B: return (extended ? Button::LEFT : Button::UNKNOWN);   // Left Arrow
        case 0x48: return (extended ? Button::UP : Button::UNKNOWN);     // Up Arrow
        case 0x4D: return (extended ? Button::RIGHT : Button::UNKNOWN);  // Right Arrow
        case 0x50: return (extended ? Button::DOWN : Button::UNKNOWN);   // Down Arrow

        case 0x47: return (extended ? Button::HOME : Button::UNKNOWN);   // Home
        case 0x4F: return (extended ? Button::END : Button::UNKNOWN);    // End
        case 0x49: return (extended ? Button::PRIOR : Button::UNKNOWN);  // Page Up
        case 0x51: return (extended ? Button::NEXT : Button::UNKNOWN);   // Page Down
        case 0x52: return (extended ? Button::INSERT : Button::UNKNOWN); // Insert
        case 0x53: return (extended ? Button::DELETE : Button::UNKNOWN); // Delete

        case 0x37: return Button::SCREEN_SHOT;   // Print Screen（需特殊处理，此处简化）
        case 0x46: return Button::SCROLL;        // Scroll Lock
        case 0x45: return Button::NUMLOCK;       // Num Lock（与Pause共享扫描码，需结合状态）
        case 0x45: return Button::PAUSE;         // Pause（实际需区分，此处简化）

        // 小键盘
        case 0x52: return Button::NUMPAD_0;      // Numpad 0
        case 0x4F: return Button::NUMPAD_1;      // Numpad 1
        case 0x50: return Button::NUMPAD_2;      // Numpad 2
        case 0x51: return Button::NUMPAD_3;      // Numpad 3
        case 0x4B: return Button::NUMPAD_4;      // Numpad 4
        case 0x4C: return Button::NUMPAD_5;      // Numpad 5
        case 0x4D: return Button::NUMPAD_6;      // Numpad 6
        case 0x47: return Button::NUMPAD_7;      // Numpad 7
        case 0x48: return Button::NUMPAD_8;      // Numpad 8
        case 0x49: return Button::NUMPAD_9;      // Numpad 9
        case 0x37: return Button::MULTIPLY;      // *
        case 0x4E: return Button::ADD;           // +
        case 0x53: return Button::SUBTRACT;      // -
        case 0x51: return Button::DECIMAL;       // .
        case 0x35: return Button::DIVIDE;        // /
        case 0x1C: return (extended ? Button::NUMPADENTER : Button::RETURN); // Numpad Enter（扩展码标识）

        default: return Button::UNKNOWN;
    }
}





}