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

Render::Render(): 
        vao(true), 
        vao_light(true),
        vbo(vertices, 8, 36, GL_STATIC_DRAW, 0), 
        program("./chap2/assets/shaders/vertex.glsl", "./chap2/assets/shaders/fragment.glsl"),
        program_light("./chap2/assets/shaders/vertex.glsl", "./chap2/assets/shaders/f_light.glsl")
{
    
    tex.emplace(
        "box", std::make_shared<fg_gl::Texture2D>("./chap2/assets/textures/container2.png", 0, true)
    );
    tex.emplace(
        "box_spec", std::make_shared<fg_gl::Texture2D>("./chap2/assets/textures/container2_specular.png", 1, true)
    );
    tex.emplace(
        "box_emission", std::make_shared<fg_gl::Texture2D>("./chap2/assets/textures/matrix.jpg", 2, true)
    );
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
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);;
}

Render::~Render(){
    program.Delete();
    program_light.Delete();
    vao.Delete();
    vao_light.Delete();
    vbo.Delete();
    for (auto& pair: tex){
        pair.second->Delete();
    }
}


void Render::Draw(){
    glm::mat4 view, projection, model;
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);  
    projection = glm::perspective(glm::radians(fov), (float)800 / (float)600, 0.1f, 100.0f);
    glm::mat4 p_v = projection * view;

    glm::vec3 LightPos(sin(current_time) * 2.0f, cos(current_time), 1.0);
    // =================================================
    // draw light cube，单纯用来说明光源的位置

    vao_light.Bind();
    program_light.Use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, LightPos);
    model = glm::scale(model, glm::vec3(0.01f));
    program_light.UniformFloatMat4Vec("model", glm::value_ptr(model));
    program_light.UniformFloatMat4Vec("p_v", glm::value_ptr(p_v));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // =================================================
    glm::vec3 BaseLight(1.0, 1.0, 1.0);
    vao.Bind();
    program.Use();
    program.UniformInt("material.diffuse", tex["box"]->CurrentUnit());
    program.UniformInt("material.specular", tex["box_spec"]->CurrentUnit());
    program.UniformInt("material.emission", tex["box_emission"]->CurrentUnit());
    program.UniformFloat("material.shininess", 128);
    program.UniformFloat3("light.position", LightPos);
    program.UniformFloat3("light.ambient", BaseLight * 0.05f);
    program.UniformFloat3("light.diffuse", BaseLight * 0.8f);
    program.UniformFloat3("light.specular", BaseLight);
    program.UniformFloat3("CameraPos", cameraPos);
    program.UniformFloat3("CameraFront", cameraFront);
    program.UniformFloatMat4Vec("p_v", glm::value_ptr(p_v));
    program.UniformFloat("time", current_time);

    for (int i=0; i<10; i++){

        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
        glm::mat3 normal = glm::mat3(glm::transpose(glm::affineInverse(model)));
        // model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        program.UniformFloatMat3Vec("normal", glm::value_ptr(normal));
        program.UniformFloatMat4Vec("model", glm::value_ptr(model));
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

}