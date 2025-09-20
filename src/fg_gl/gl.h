#pragma once

extern "C"{
#include <glad/gl.h>

#ifdef _WIN32
#define GLAD_WGL_IMPLEMENTATION
#include <glad/wgl.h> 
#undef GLAD_WGL_IMPLEMENTATION
#endif
}

#ifdef _WIN32
#include<Windows.h>
#endif

#include <string>

namespace fg_gl{

#ifdef _WIN32
bool GLInit(HWND hwnd, HDC hdc);
#endif

struct ShaderProgram{
    ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path);
    ~ShaderProgram();

    unsigned int ProgramID();
    void Use();

    void UniformBool(const std::string &name, bool value) const;
    void UniformInt(const std::string &name, int value) const;
    void UniformFloat(const std::string &name, float value) const;
    void UniformFloat3(const std::string &name, float a, float b, float c);

private:
    unsigned int program_;
};

struct Texture2D {
    Texture2D(const char* path, unsigned int tex_unit, unsigned int org_fmt, unsigned int target_fmt);
    void BindUnit(unsigned int tex_unit);
    unsigned int CurrentUnit() const;
private:
    unsigned int texture_;
    unsigned int tex_unit_;
    int width_;
    int height_;
    int channel_;
};

}