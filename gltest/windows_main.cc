#include "render.h"

#include <iostream>
#include <cstdio>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600


int main(){
    // fg_utils::CreateDebugConsole();
    auto window = fg_window::Window(L"Game", SCR_WIDTH, SCR_HEIGHT);

    Render render;

    fg_interact::Event event;
    while(true){
        if(fg_interact::PollEvent(&event)){
            std::cout << "Main Loop Get Event: "
                    << event.type << std::endl;
            if (event.type == fg_interact::EventType::kExitEvent) exit(0);
        }

        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(1.0f, 1,0f, 1.0f, 1.0f);
        // 这里，如果没有 GL_DEPTH_BUFFER_BIT 会导致无法正常显示，
        // 我以为只会导致 zbuffer 没清空，使得显示错位，没想到直接就不输出了，离谱，害我 debug 半天
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render.Draw();
        window.SwapBuffer();

    }
}