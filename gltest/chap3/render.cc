#include "render.h"
#include <chrono>
#include <iostream>



Render::Render(){
    program.Init("chap3/assets/shaders/vertex.glsl", "chap3/assets/shaders/frag.glsl");
    VLOG(1) << "start load model";
    bag_model.Load("chap3/assets/bag/backpack.obj");
    VLOG(1) << "end load model";
    glEnable(GL_DEPTH_TEST);  
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);;
}

int cnt = 0;

void Render::Draw(){
    glm::mat4 view, projection, model;
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);  
    projection = glm::perspective(glm::radians(fov), (float)800 / (float)600, 0.1f, 100.0f);
    glm::mat4 p_v = projection * view;

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

    // =================================================
    program.Use();
    program.UniformFloatMat4Vec("p_v", glm::value_ptr(p_v));
    program.UniformFloatMat4Vec("model", glm::value_ptr(model));

    // bag_model.DrawNode("Plane.008__0", program);
    // bag_model.DrawNode("Cylinder.049__0", program);
    bag_model.DrawRoot(program);
}