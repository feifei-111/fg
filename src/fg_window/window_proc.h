#pragma once

#ifdef _WIN32
#include <Windows.h>

namespace fg_window{

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
}
#endif