#pragma once

#include <Windows.h>

namespace fg::window::win32 {

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

}
