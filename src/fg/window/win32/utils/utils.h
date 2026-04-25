#pragma once
#include <Windows.h>
#include <string>

namespace fg::window::win32::utils {
std::wstring ToWString(const char* str);
}
