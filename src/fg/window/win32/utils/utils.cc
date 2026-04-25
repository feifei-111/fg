#include "fg/window/win32/utils/utils.h"

namespace fg::window::win32::utils {

std::wstring ToWString(const char* str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    std::wstring result(len - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, result.data(), len);
    return result;
}

}  // namespace fg::window::win32::utils
