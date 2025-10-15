#include "render.h"
#include <chrono>
#include <iostream>



Render::Render(){
    program.Init("chap3/assets/texture/vertex.glsl", "chap3/assets/texture/frag.glsl");
    bag_model.Load("chap3/assets/bag/backpack.obj");
    glEnable(GL_DEPTH_TEST);  
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);;
}


void Render::Draw(){
    glm::mat4 view, projection, model;
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);  
    projection = glm::perspective(glm::radians(fov), (float)800 / (float)600, 0.1f, 100.0f);
    glm::mat4 p_v = projection * view;

    // =================================================
    program.Use();
    program.UniformFloatMat4Vec("p_v", glm::value_ptr(p_v));

    bag_model.Draw(program);
}