#pragma once
#include <fg/window/event.h>
#include <fg/window/window.h>

// 提供一个 lib 内部可用的 mutable state 获取方式
namespace fg::window {
WindowState* GetMutableState(WindowBase* window);
}  // namespace fg::window

// 提供 msg loop push event 的接口
namespace fg::window::event {
bool PushEvent(Event* event);
Event* PrepareNextEvent(unsigned int window_id);
bool PrepareEventDone();

void SetFetchEvent(void (*hook)());
}  // namespace fg::window::event
