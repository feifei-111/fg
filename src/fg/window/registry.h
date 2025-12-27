#pragma once
#include <fg/macros.h>
#include <fg/window/window.h>

namespace fg::window {

struct WindowRegisterInfo {
    unsigned int id;
    WindowBase* window;
};

void FG_API RegisterWindow(const WindowRegisterInfo& reg_info);
void FG_API UnregisterWindow(unsigned int window_id);
WindowBase* GetRegisteredWindowPtr(unsigned int window_id);

}