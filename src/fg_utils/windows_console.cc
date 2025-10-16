#ifdef _WIN32

#include "fg_utils/utils.h"

#include <Windows.h>
#include <cstdio>
#include <io.h>
#include <fcntl.h>

namespace fg_utils{

void CreateDebugConsole()
{
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

}

#endif