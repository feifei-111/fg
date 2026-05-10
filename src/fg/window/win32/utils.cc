#include "fg/window/win32/utils.h"

#include <Windows.h>
#include <fcntl.h>
#include <fg/utils/utils.h>
#include <io.h>
#include <cstdio>

namespace fg::window::win32::utils {

std::wstring ToWString(const char* str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    std::wstring result(len - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, result.data(), len);
    return result;
}

void CreateConsole() {
    static bool created = false;
    if (created) return;
    created = true;

    AllocConsole();

    HANDLE hStdHandle;
    int hConHandle;
    FILE* fp;

    hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle((intptr_t)hStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    freopen_s(&fp, "CONOUT$", "w", stdout);
    setvbuf(stdout, NULL, _IONBF, 0);

    hStdHandle = GetStdHandle(STD_ERROR_HANDLE);
    hConHandle = _open_osfhandle((intptr_t)hStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    freopen_s(&fp, "CONOUT$", "w", stderr);
    setvbuf(stderr, NULL, _IONBF, 0);

    hStdHandle = GetStdHandle(STD_INPUT_HANDLE);
    hConHandle = _open_osfhandle((intptr_t)hStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "r");
    freopen_s(&fp, "CONIN$", "r", stdin);
    setvbuf(stdin, NULL, _IONBF, 0);

    SetConsoleTitle(L"FEI_console");
}

}  // namespace fg::window::win32::utils
