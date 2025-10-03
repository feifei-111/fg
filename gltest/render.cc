#include "render.h"
#include <chrono>
#include <iostream>


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

unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};


Render::Render(): 
        vao(), 
        vbo(vertices, 5, 36, GL_FLOAT, GL_STATIC_DRAW, 0), 
        program("./assets/shaders/vertex.glsl", "./assets/shaders/fragment.glsl"),
        tex0("./assets/textures/awesomeface.png", 0, GL_RGB, GL_RGBA),
        tex1("./assets/textures/wall.jpg", 3, GL_RGB, GL_RGB)
    {
    vbo.SetAttr(3, GL_FALSE);
    vbo.SetAttr(2, GL_FALSE);

    vao.UnBind();
    vbo.UnBind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glEnable(GL_DEPTH_TEST);  
}

void Render::Draw(){
    std::cout << "draw" << std::endl;
    float timeValue = fg_utils::GetTime();
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
    projection = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);

    for (int i=0; i<10; i++){
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        model = glm::rotate(model, timeValue * (i+1), glm::vec3(0.5f, 1.0f, 0.0f));
        glm::mat4 trans = projection * view * model;
        program.UniformFloatMat4Vec("trans", glm::value_ptr(trans));
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        std::cout << "glDrawArrays" << std::endl;
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

}


void Render::Draw2(){

    vao.Bind();
    program.Use();
    program.UniformInt("Tex0", tex0.CurrentUnit());
    program.UniformInt("Tex1", tex1.CurrentUnit());
    
    glm::mat4 view;
    glm::mat4 projection;

    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);  
    projection = glm::perspective(glm::radians(fov), (float)800 / (float)600, 0.1f, 100.0f);

    for (int i=0; i<10; i++){
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(0.5f, 1.0f, 0.0f));
        glm::mat4 trans = projection * view * model;
        program.UniformFloatMat4Vec("trans", glm::value_ptr(trans));
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}