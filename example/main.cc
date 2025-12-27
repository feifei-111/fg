#include <iostream>
#include <fg/fg.h>


float vertices[] = {
    -0.5f, -0.5f, 0.0f, // left  
        0.5f, -0.5f, 0.0f, // right 
        0.0f,  0.5f, 0.0f  // top   
}; 

int main(){
    // 1. Prepare window and init opengl env
    fgi::WindowConfig config;
    config.name = "MAIN";
    config.width = 800;
    config.height = 600;

    auto window = fgi::CreateWindow(config);

    // 2. Prepare opengl data for rendering
    fgi::ShaderProgram program(
        fgi::GetRuntimeAbsPath("./assets/vertex_shader.glsl"), 
        fgi::GetRuntimeAbsPath("./assets/fragment_shader.glsl"));
    std::cout << program.Ready() << std::endl;

    fgi::VAO vao(true/*do init*/);
    fgi::VBO vbo(vertices, 3, 3, GL_STATIC_DRAW, 0);

    vao.Bind();
    vbo.Bind();
    vbo.SetAttr(3, GL_FALSE/*normalize*/);
    vao.UnBind();
    vbo.UnBind();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // 3. Render loop
    fgi::Event event;

    while(true){
        // process event
        fgi::CollectEvents();
        while(fgi::PollEvent(&event)){
            std::cout << "Main Loop Get Event: " << event.TypeStr() << std::endl;
            if (
                std::holds_alternative<fgi::ExitEvent>(event.data) ||
                std::holds_alternative<fgi::KeyBoardEvent>(event.data) &&
                    std::get<fgi::KeyBoardEvent>(event.data).button == fgi::Button::ESC
            ) exit(0);
        }

        // rendering
        glClear(GL_COLOR_BUFFER_BIT);
        program.Use();
        program.UniformFloat("time", fgi::GetTime());
        vao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // show
        window->SwapBuffer();
    }

    program.Delete();
    vao.Delete();
    vbo.Delete();
}