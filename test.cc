#include "fg_window/window.h"
#include "fg_event/event.h"
#include "fg_gl/gl.h"

#include "debug.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstdio>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

int main(){

    CreateDebugConsole();

    auto window = fg_window::Window(L"Game", SCR_WIDTH, SCR_HEIGHT);

    glClearColor(0.0, 0.5, 0.0, 1.0);

    fg_gl::ShaderProgram program = fg_gl::ShaderProgram(
        "assets/shaders/simple_v.glsl", "assets/shaders/simple_f.glsl");

    float vertices[] = {
        -0.5f, -0.5f, -0.0f,
         0.5f, -0.5f, -0.0f, 
         0.0f,  0.5f, -0.0f
    };

    fg_gl::VAO vao;

    fg_gl::VBO<float> vbo(vertices, 3, 3, GL_FLOAT, GL_STATIC_DRAW, 0);
    vbo.SetAttr(3);

    vao.UnBind();
    vbo.UnBind();


    fg_event::Event event;

    float timeValue = 0;
    while(true){
        while(fg_event::PollEvent(&event)){
            printf("main: %x, %x, %x\n",event.data.common_windows_event.win_msg, event.data.common_windows_event.lparam, event.data.common_windows_event.wparam);

            glClear(GL_COLOR_BUFFER_BIT);

            program.Use();
            vao.Bind();

            glDrawArrays(GL_TRIANGLES, 0, 3);
            window.SwapBuffer();
        }
    }
}