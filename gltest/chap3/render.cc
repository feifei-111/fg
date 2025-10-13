#include "render.h"
#include <chrono>
#include <iostream>



Render::Render(){

    glEnable(GL_DEPTH_TEST);  
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);;
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