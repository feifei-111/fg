#include "window/window.h"
#include "event/event.h"

#include "console.h"

#include<stdio.h>

int main(){

    CreateDebugConsole();
    printf("start\n");
    auto window = fei_window::Window(L"Game", 1280, 720);

    fei_window::Event event;
    while(true){
        while(fei_window::PollEvent(&event)){
            printf("main: %x, %x, %x\n",event.common_windows_event.win_msg, event.common_windows_event.lparam, event.common_windows_event.wparam);

            Sleep(100);
        }
    }

}