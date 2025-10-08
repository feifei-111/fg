#include "render.h"
#include <chrono>
#include <iostream>


float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f
};



Render::Render(): 
        vao(), 
        vao_light(),
        vbo(vertices, 8, 36, GL_FLOAT, GL_STATIC_DRAW, 0), 
        program("./assets/shaders/vertex.glsl", "./assets/shaders/fragment.glsl"),
        program_light("./assets/shaders/vertex.glsl", "./assets/shaders/f_light.glsl"),
        tex0("./assets/textures/awesomeface.png", 0, GL_RGB, GL_RGBA),
        tex1("./assets/textures/wall.jpg", 3, GL_RGB, GL_RGB)
{
    vao.Bind();
    vbo.Bind();
    vbo.SetAttr(3, GL_FALSE);
    vbo.SetAttr(2, GL_FALSE);
    vbo.SetAttr(3, GL_TRUE);
    vao.UnBind();
    vbo.UnBind();

    vbo.ResetAttr();
    vao_light.Bind();
    vbo.Bind();
    vbo.SetAttr(3, GL_FALSE);
    vbo.SetAttr(2, GL_FALSE);
    vbo.SetAttr(3, GL_TRUE);
    vao_light.UnBind();
    vbo.UnBind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glEnable(GL_DEPTH_TEST);  
}


void Render::Draw(){
    glm::mat4 view, projection, model;
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);  
    projection = glm::perspective(glm::radians(fov), (float)800 / (float)600, 0.1f, 100.0f);
    glm::mat4 p_v = projection * view;

    glm::vec3 LightPos(sin(current_time) * 2.0f, cos(current_time), 1.0);
    glm::vec3 CubePos(0.0, 0.0, 0.0);
    // =================================================
    // draw light cube，单纯用来说明光源的位置
    vao_light.Bind();
    program_light.Use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, LightPos);
    model = glm::scale(model, glm::vec3(0.1f));
    program.UniformFloatMat4Vec("model", glm::value_ptr(model));
    program.UniformFloatMat4Vec("p_v", glm::value_ptr(p_v));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // =================================================
    vao.Bind();
    program.Use();
    program.UniformInt("Tex0", tex0.CurrentUnit());
    program.UniformInt("Tex1", tex1.CurrentUnit());
    program.UniformFloat3("LightPos", LightPos.x, LightPos.y, LightPos.z);
    program.UniformFloat3("LightColor", 1.0, 1.0, 1.0);
    program.UniformFloat3("ObjColor", 1.0, 0.5, 0.31);
    program.UniformFloat3("CameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

    model = glm::mat4(1.0f);
    model = glm::translate(model, CubePos);

    glm::mat3 normal = glm::mat3(glm::transpose(glm::affineInverse(model)));
    // model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.5f, 1.0f, 0.0f));
    program.UniformFloatMat3Vec("normal", glm::value_ptr(normal));
    program.UniformFloatMat4Vec("model", glm::value_ptr(model));
    program.UniformFloatMat4Vec("p_v", glm::value_ptr(p_v));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // =================================================
}