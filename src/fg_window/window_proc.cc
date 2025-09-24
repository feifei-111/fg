#include <iostream>

#include "fg_window/window.h"
#include "fg_window/window_proc.h"
#include "fg_interact/event.h"
#include "fg_utils/utils.h"


namespace fei_window {

static void RecordMouseMove(WPARAM wParam, LPARAM lParam, WindowState* state);

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
            fg_interact::Event event;
            event.type = fg_interact::EventType::kExitEvent;
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
            fg_interact::Event event;
            event.type = fg_interact::EventType::kPaintEvent;
            PushEvent(&event);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        case WM_MOUSEMOVE:
            data->state.mouse_ts = fg_utils::GetTime();
            data->state.mouse_lbutton = wParam & MK_LBUTTON;
            data->state.mouse_rbutton = wParam & MK_RBUTTON;
            data->state.mouse_mbutton = wParam & MK_MBUTTON;
            data->state.mouse_shift = wParam & MK_SHIFT;
            data->state.mouse_ctrl = wParam & MK_CONTROL;
            data->state.mouse_xbutton1 = wParam & MK_XBUTTON1;
            data->state.mouse_xbutton2 = wParam & MK_XBUTTON2;
            data->state.mouse_x = LOWORD(lParam);
            data->state.mouse_y = HIWORD(lParam);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
        default:
            fg_interact::Event event;
            event.type = fg_interact::EventType::kCommonEvent;
            event.data.common_event.win_msg = uMsg;
            event.data.common_event.wparam = wParam;
            event.data.common_event.lparam = lParam;
            PushEvent(&event);
            return DefWindowProcW(hwnd, uMsg, wParam, lParam); // 默认消息处理
    }
}


void RecordMouseMove(WPARAM wParam, LPARAM lParam, WindowState* state){
    
    state->
}


}