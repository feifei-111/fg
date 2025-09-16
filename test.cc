#include "window/window.h"
#include "event/event.h"

#include "debug.h"
#include "Windows.h"

extern "C" {
#include "glad/gl.h"
}

#include<stdio.h>

// vertex shader
const char *vertexShaderSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// fragment shader
const char *fragmentShaderSource = 
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main(){

    CreateDebugConsole();
    printf("start\n");
    auto window = fei_window::Window(L"Game", 1280, 720);

    int success;
    char infoLog[512];

    // compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("%s\n", infoLog);
        exit(1);
    }

    // compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("%s\n", infoLog);
        exit(1);
    }

    // create program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("%s\n", infoLog);
        exit(1);
    }

    // link 到 program 之后就没用了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 打印一个简单的三角形
    // 创建 vertex 以及 buffer
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    }; 

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 记录 VAO
    unsigned int VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 注意，VAO 记录了 VBO，所以使用 VAO 会自动使用记录的 VBO，VBO 可以直接解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    
    // VAO 暂时解绑，需要时再用
    glBindVertexArray(0); 


    // 设置 clear 时的默认背景色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    fei_window::Event event;
    while(true){
        while(fei_window::PollEvent(&event)){
            printf("main: %x, %x, %x\n",event.common_windows_event.win_msg, event.common_windows_event.lparam, event.common_windows_event.wparam);
            // 按左键
            // if (event.common_windows_event.win_msg == WM_LBUTTONDOWN){

            // }
            glClear(GL_COLOR_BUFFER_BIT);
            
            // 进行绘制，设置 program，VAO 包含了 VBO，所以 bind VAO 足够了
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            // 绘制 VBO 中的前三个 vertex 构成的三角形
            glDrawArrays(GL_TRIANGLES, 0, 3);

            window.SwapBuffer();
        }
    }

    // 清理数据
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}