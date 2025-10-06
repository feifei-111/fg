#pragma once

extern "C"{
#include <glad/gl.h>

#ifdef _WIN32
#include <glad/wgl.h> 
#endif
}

#ifdef _WIN32
#include<Windows.h>
#endif

#include <string>
#include <iostream>

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
    void UniformFloatMat4Vec(const std::string &name, float* data, size_t mat_num=1, unsigned int transpose=GL_FALSE);

private:
    unsigned int program_;
};

struct Texture2D {
    Texture2D(const char* path, unsigned int tex_unit, unsigned int org_fmt, unsigned int target_fmt, bool flip_load=true);
    void BindUnit(unsigned int tex_unit);
    unsigned int CurrentUnit() const;
private:
    unsigned int texture_;
    unsigned int tex_unit_;
    int width_;
    int height_;
    int channel_;
};

template <typename Dtype>
struct VBO{
    VBO(
        Dtype* data, 
        unsigned int column_num, 
        unsigned int vertex_num, 
        unsigned int gl_dtype, 
        unsigned int draw_type, 
        unsigned int attr_begin=0)
        :attr_idx_(attr_begin), column_num_(column_num), vertex_num_(vertex_num), gl_dtype_(gl_dtype), column_idx_(0)
    {
        stride_ = sizeof(Dtype) * column_num_;
        size_t data_size = stride_ * vertex_num_;
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, data_size, data, draw_type);
    }

    void SetAttr(int column_size, unsigned int norm=GL_FALSE){
        glVertexAttribPointer(attr_idx_, column_size, gl_dtype_, norm, stride_, (void*)column_idx_);
        glEnableVertexAttribArray(attr_idx_);
        attr_idx_++;
        column_idx_ += column_size;
    }
    void Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    }

    void UnBind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    ~VBO(){
        UnBind();
        glDeleteBuffers(1, &vbo_);
    }
private:
    unsigned int vbo_;
    int attr_idx_;
    Dtype* column_idx_;
    int column_num_;
    int vertex_num_;
    unsigned int gl_dtype_;
    size_t stride_;
};

struct EBO{
    EBO(unsigned int* data, unsigned int data_size, unsigned int draw_type);
    void Bind() const;
    void UnBind() const;
    ~EBO();
private:
    unsigned int ebo_;
};

struct VAO{
    VAO();
    void Bind() const;
    void UnBind() const;
    ~VAO();
private:
    unsigned int vao_;
};

}