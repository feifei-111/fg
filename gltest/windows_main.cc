#include "render.h"

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
                    << event.type << std::endl;
        }
        float time = fg_utils::GetTime();
        // glClearColor(0.0, sin(time), 0.0, 1.0);
        // glClear(GL_COLOR_BUFFER_BIT);
        render.Draw();
        window.SwapBuffer();

    }
}