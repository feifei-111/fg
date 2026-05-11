#pragma once
#include <string>

namespace fg::window::win32::utils {
void CreateConsole();
std::wstring ToWString(const char* str);
}  // namespace fg::window::win32::utils
