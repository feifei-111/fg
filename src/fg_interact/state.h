#pragma once

namespace fg_interact{

enum KeyBoardButton {
    
}


// 记录常规状态，主要是按键按下状态，鼠标位置等
struct MouseState {
    float time_stamp;

    int mouse_x;
    int mouse_y;

    bool mouse_lbutton :1 ;
    bool mouse_rbutton :1 ;
    bool mouse_mbutton :1 ; // 滚轮按下
    bool mouse_shift :1 ;   // 这个状态其实是 keyboard 导致的，但是我们还是记录一下，因为 mouse 相关的 msg 有这个信息，而且与 kerboard 不一定同步
    bool mouse_ctrl :1 ;    // 同上
    bool mouse_xbutton1 :1 ;      // x1 x2 两个侧键
    bool mouse_xbutton2 :1 ;
};

struct KeyboardState {
    // 时间戳（记录状态发生的时间）
    float time_stamp = 0;

    // 修饰键状态（左右键分开记录，更精细）
    bool shift_left = false;    // 左Shift
    bool shift_right = false;   // 右Shift
    bool ctrl_left = false;     // 左Ctrl
    bool ctrl_right = false;    // 右Ctrl
    bool alt_left = false;      // 左Alt
    bool alt_right = false;     // 右Alt
    bool win_left = false;      // 左Win键（Windows键）
    bool win_right = false;     // 右Win键

    // 普通按键状态（键码 -> 按下状态，使用平台无关的虚拟键码）
    // 键码建议采用 SDL 或 GLFW 的跨平台虚拟键码标准（如 SDLK_a, SDLK_ESCAPE 等）
    u_int8_t button_map[256];

    // 最近输入的字符（Unicode，适用于 WM_CHAR 类消息）
    wchar_t input_char = 0;
    bool is_dead_char = false;  // 是否为死键（如重音符号）

    // 辅助信息：按键重复次数（仅用于按下重复场景）
    uint16_t repeat_count = 0;
};

struct WindowState{
    MouseState mouse_state;
};

}
