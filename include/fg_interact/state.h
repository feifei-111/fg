#pragma once

namespace fg_interact {

enum class Button {
    // mouse part
    MS_LBUTTON,
    MS_RBUTTON,
    MS_MBUTTON,
    MS_XBUTTON1,
    MS_XBUTTON2,

    // keyboard part
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    // numbers
    NUM_0,
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
    NUM_6,
    NUM_7,
    NUM_8,
    NUM_9,

    // alphabets
    LETTER_A,
    LETTER_B,
    LETTER_C,
    LETTER_D,
    LETTER_E,
    LETTER_F,
    LETTER_G,
    LETTER_H,
    LETTER_I,
    LETTER_J,
    LETTER_K,
    LETTER_L,
    LETTER_M,
    LETTER_N,
    LETTER_O,
    LETTER_P,
    LETTER_Q,
    LETTER_R,
    LETTER_S,
    LETTER_T,
    LETTER_U,
    LETTER_V,
    LETTER_W,
    LETTER_X,
    LETTER_Y,
    LETTER_Z,

    // 控制
    BACK,
    RETURN,
    ESC,

    // 符号 button
    BACK_QUOTE,  // 反引号，esc 下面的
    MINUS,       // 减号
    EQUAL,       // 等号

    TAB,
    L_SQUARE_BRACKET,  // 左右方括号
    R_SQUARE_BRACKET,
    BACKSLASH,  // 反斜杠

    SEMICOLON,  // 分号
    QUOTE,      // 单引号

    COMMA,   // 逗号
    PERIOD,  // 句号
    SLASH,   // 斜杠，除法

    SPACE,

    // 组合键
    CAPITAL,
    LSHIFT,
    RSHIFT,
    LCONTROL,
    RCONTROL,
    LMENU,  // alt
    RMENU,
    LWIN,
    RWIN,

    // 方向键
    LEFT,
    UP,
    RIGHT,
    DOWN,

    // 功能键
    HOME,
    END,
    PRIOR,
    NEXT,
    INSERT,
    DEL,
    SCREEN_SHOT,
    SCROLL,  // 基本被淘汰的功能
    PAUSE,

    // 小键盘的数字以及运算符
    NUMLOCK,
    NUMPAD_0,
    NUMPAD_1,
    NUMPAD_2,
    NUMPAD_3,
    NUMPAD_4,
    NUMPAD_5,
    NUMPAD_6,
    NUMPAD_7,
    NUMPAD_8,
    NUMPAD_9,
    MULTIPLY,
    ADD,
    SEPARATOR,  // 分隔符，比如 100,000 这里的逗号
    SUBTRACT,
    DECIMAL,  // 小数点
    DIVIDE,
    // NUMPADENTER, 小键盘 return 不区分了

    UNKNOWN
};

enum class ButtonMove { DOWN, UP, DCLICK };

struct WindowState {
    float time_stamp;

    bool button_map[256];

    int mouse_x;
    int mouse_y;
};

}  // namespace fg_interact
