#include "rend.h"

#include <iostream>
#include <cstdio>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

int main(){
    fg_utils::CreateDebugConsole();
    auto window = fg_window::Window(L"Game", SCR_WIDTH, SCR_HEIGHT);

    Render render;

    fg_interact::Event event;
    while(true){
        if(fg_interact::PollEvent(&event)){
            std::cout << "Main Loop Get Event: "
                    << event.data.common_windows_event.win_msg << " "
                    << event.data.common_windows_event.lparam << " "
                    << event.data.common_windows_event.wparam << std::endl;

            // Update render

        }
        glClear(GL_COLOR_BUFFER_BIT);
        render.Draw();
        window.SwapBuffer();
    }
}