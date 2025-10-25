#include "render.h"
#include <chrono>
#include <iostream>



Render::Render(int argc, char** argv){
    
    std::cout << std::endl;
    if (argc > 1){
        std::cout << "load "<< argv[1] << std::endl;
        scene.Load(argv[1]);
        // 模型如果没有设置 mtl，可能加载不到 tex
        program.Init("chap3/assets/shaders/vertex.glsl", "chap3/assets/shaders/frag_no_diffuse.glsl");
    }else{
        std::cout << "use default model" << std::endl;
        scene.Load("chap3/assets/bag/backpack.obj");
        program.Init("chap3/assets/shaders/vertex.glsl", "chap3/assets/shaders/frag.glsl");
    }
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

    scene.DrawRoot(program);
}