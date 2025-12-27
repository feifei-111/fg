#pragma once
#include <fg/window/window.h>

namespace fg::window {

// 提供一个 lib 内部可用的 mutable state 获取方式
WindowState* GetMutableState(WindowBase* window);

}  // namespace fg::window
