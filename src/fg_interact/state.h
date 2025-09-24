#pragma once

namespace fg_interact{

// 记录常规状态，主要是按键按下状态，鼠标位置等
struct MouseState {
    float timestamp=-1;

    bool mouse_lbutton;
    bool mouse_rbutton;
    bool mouse_mbutton; // 滚轮按下
    bool mouse_shift;   // 这个状态其实是 keyboard 导致的，但是我们还是记录一下，因为 mouse 相关的 msg 有这个信息，而且与 kerboard 不一定同步
    bool mouse_ctrl;    // 同上
    bool mouse_xbutton1;      // x1 x2 两个侧键
    bool mouse_xbutton2;

    // 鼠标位置信息
    int mouse_x;
    int mouse_y;
};

struct WindowState{
    MouseState mouse_state;
};

}
