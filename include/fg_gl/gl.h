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
#include <glm/glm.hpp>

namespace fg_gl{

#ifdef _WIN32
bool GLInit(HWND hwnd, HDC hdc);
#endif

struct ShaderProgram{
    ShaderProgram() = default;
    ShaderProgram(const std::string& vertex_shader_path, const std::string& fragment_shader_path);

    bool Init(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
    bool Ready() const {
        return program_ != 0;
    }
    unsigned int ProgramID() const {
        return program_;
    }
    void Use(){
        glUseProgram(program_); 
    }
    void UniformInt(const std::string &name, int value) const{
        glUniform1i(glGetUniformLocation(program_, name.c_str()), value); 
    }
    void UniformFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(program_, name.c_str()), value); 
    }
    void UniformFloat3(const std::string &name, float v1, float v2, float v3){
        glUniform3f(glGetUniformLocation(program_, name.c_str()), v1, v2, v3); 
    }
    void UniformFloat3(const std::string &name, const glm::vec3& vec3){
        glUniform3f(glGetUniformLocation(program_, name.c_str()), vec3.x, vec3.y, vec3.z); 
    }
    void UniformFloatMat3Vec(const std::string &name, float* data, size_t mat_num=1, unsigned int transpose=GL_FALSE){
        glUniformMatrix3fv(glGetUniformLocation(program_, name.c_str()), mat_num, transpose, data);
    }
    void UniformFloatMat4Vec(const std::string &name, float* data, size_t mat_num=1, unsigned int transpose=GL_FALSE){
        glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), mat_num, transpose, data);
    }

    ~ShaderProgram(){
        glDeleteProgram(program_);
    }
private:
    unsigned int program_ = 0;
};

struct Texture2D {
    static void SimpleSetParameter(){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }

    Texture2D() = default;
    Texture2D(const std::string& path, unsigned int tex_unit=0, bool flip_load=false);

    bool Init(const std::string& path, unsigned int tex_unit=0, bool flip_load=false);
    bool Ready() const {
        return texture_ != 0;
    }
    void BindUnit(unsigned int tex_unit);
    unsigned int CurrentUnit() const {
        return tex_unit_;
    }

    ~Texture2D(){
        glDeleteTextures(1, &texture_);
    }
private:
    unsigned int texture_ = 0;
    unsigned int tex_unit_;
    int width_;
    int height_;
    int channel_;
};

template <typename Dtype>
struct GLDtype{};

template<>
struct GLDtype<float>{
    static constexpr unsigned int value=GL_FLOAT;
};

template <typename Dtype>
struct VBOTemplate{
    VBOTemplate() = default;
    VBOTemplate(
        Dtype* data, 
        unsigned int column_num, 
        unsigned int vertex_num, 
        unsigned int draw_type, 
        unsigned int attr_begin=0)
    {
        Init(data, column_num, vertex_num, draw_type, attr_begin);
    }
    bool Init(
        Dtype* data, 
        unsigned int column_num, 
        unsigned int vertex_num, 
        unsigned int draw_type, 
        unsigned int attr_begin=0)
    {
        attr_idx_ = attr_begin;
        column_num_ = column_num;
        vertex_num_ = vertex_num;
        column_idx_ = 0;

        stride_ = sizeof(Dtype) * column_num_;
        size_t data_size = stride_ * vertex_num_;
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, data_size, data, draw_type);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return true;
    }

    bool Ready() const {
        return vbo_ != 0;
    }

    // if reuse same vbo
    void ResetAttr(int attr_idx=0, int column_idx=0){
        attr_idx_ = attr_idx;
        column_idx_ = (Dtype*)0 + column_idx;
    }

    void SetAttr(int column_size, unsigned int norm=GL_FALSE){
        glVertexAttribPointer(attr_idx_, column_size, GLDtype<Dtype>::value, norm, stride_, (void*)column_idx_);
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

    ~VBOTemplate(){
        UnBind();
        glDeleteBuffers(1, &vbo_);
    }
private:
    unsigned int vbo_ = 0;
    int attr_idx_;
    Dtype* column_idx_;
    int column_num_;
    int vertex_num_;
    size_t stride_;
};

using VBO=VBOTemplate<float>;

struct EBO{
    EBO() = default;
    EBO(unsigned int* data, unsigned int data_size, unsigned int draw_type);

    bool Init(unsigned int* data, unsigned int data_size, unsigned int draw_type);
    bool Ready() const;
    void Bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    }
    void UnBind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    ~EBO(){
        glDeleteBuffers(1, &ebo_);
    }
private:
    unsigned int ebo_;
};

struct VAO{
    VAO(){
        glGenVertexArrays(1, &vao_);
    }

    bool Ready() const {
        return vao_ != 0;
    }
    void Bind() const {
        glBindVertexArray(vao_);
    }
    void UnBind() const{
        glBindVertexArray(0);
    }
    ~VAO(){
        glDeleteVertexArrays(1, &vao_);
    }
private:
    unsigned int vao_;
};

}