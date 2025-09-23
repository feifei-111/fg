#include "window/window.h"
#include "event/event.h"

#include "debug.h"
#include "Windows.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern "C" {
#include "glad/gl.h"
}

#include<stdio.h>

int main(){

    CreateDebugConsole();
    printf("start\n");

    auto window = fg_window::Window(L"Game", 1280, 720);

    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    fg_gl::VAO vao;

    fg_gl::VBO<float> vbo(vertices, 5, 36, GL_FLOAT, GL_STATIC_DRAW, 0);
    vbo.SetAttr(3, GL_FALSE);
    vbo.SetAttr(2, GL_FALSE);
    // fg_gl::EBO ebo(indices, sizeof(indices), GL_STATIC_DRAW);

    // end record
    vao.UnBind();
    vbo.UnBind();
    // ebo.UnBind();


    fg_gl::ShaderProgram program = fg_gl::ShaderProgram(
        "assets/shaders/vertex.glsl", "assets/shaders/fragment.glsl");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    fg_gl::Texture2D tex0("assets/textures/awesomeface.png", 0, GL_RGB, GL_RGBA);
    fg_gl::Texture2D tex1("assets/textures/wall.jpg", 3, GL_RGB, GL_RGB);

    glEnable(GL_DEPTH_TEST);  

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    fg_window::Event event;

    float timeValue = 0;
    while(true){
        while(fg_window::PollEvent(&event)){
            printf("main: %x, %x, %x\n",event.common_windows_event.win_msg, event.common_windows_event.lparam, event.common_windows_event.wparam);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            timeValue += 100;
            const float radius = 10.0f;
            float camX = sin(timeValue) * radius;
            float camZ = cos(timeValue) * radius;
    
            vao.Bind();
            program.Use();
    
            program.UniformInt("Tex0", tex0.CurrentUnit());
            program.UniformInt("Tex1", tex1.CurrentUnit());
    
            
            glm::mat4 view;
            glm::mat4 projection;
            view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));  
            projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    
            for (int i=0; i<10; i++){
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                model = glm::rotate(model, timeValue * (i+1), glm::vec3(0.5f, 1.0f, 0.0f));
                glm::mat4 trans = projection * view * model;
                program.UniformFloatMat4Vec("trans", glm::value_ptr(trans));
                // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }    

            window.SwapBuffer();
        }
    }
}