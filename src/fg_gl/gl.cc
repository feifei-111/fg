#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

extern "C"{
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION

#ifdef _WIN32
#define GLAD_WGL_IMPLEMENTATION
#include <glad/wgl.h> 
#undef GLAD_WGL_IMPLEMENTATION
#endif
}

#include "fg_gl/gl.h"

#include <iostream>
#include <fstream>
#include <sstream>


namespace fg_gl{
    
#ifdef _WIN32
// 关于 log，后面再加
bool GLInit(HWND hwnd, HDC hdc){

    // 设置像素格式，并绑到 hdc
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), // 结构体大小
        1,                            // 版本号
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // flags，包括双缓冲
        PFD_TYPE_RGBA,                 // RGBA 颜色类型
        32,                            // 32 位颜色深度
        0, 0, 0, 0, 0, 0,              // 忽略颜色位
        0,                             // 无 alpha 缓冲区
        0,                             // 忽略移位位
        0,                             // 无累积缓冲区
        0, 0, 0, 0,                    // 忽略累积位
        24,                            // 24 位深度缓冲区
        8,                             // 8 位模板缓冲区
        0,                             // 无辅助缓冲区
        PFD_MAIN_PLANE,                // 主层
        0,                             // 保留
        0, 0, 0                        // 忽略层掩码
    };

    // 这个 choose 接口是 windows sdk 提供的
    // opengl 也有可扩展的像素格式，wglChoosePixelFormatARB
    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    // 初始化 glad，因为 opengl 要求必须在 rc 非空的情况下才能 load 目标函数，所以我们需要一个 temp rc
    // 创建 temp rendering context
    // wglCreateContext 这个接口只能创建 1.x 版本的 rc
    HGLRC temp_rc = wglCreateContext(hdc);
    if(!temp_rc){
        std::cout << "temp_rc" << std::endl;
        return false;
    }

    // 设置成当前的渲染设备
    if(!wglMakeCurrent(hdc, temp_rc)){
        std::cout << "wglMakeCurrent" << std::endl;
        return false;
    }

    // 设置 rc 之后可以加载 extension 函数
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = 
    (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    if(!wglCreateContextAttribsARB){
        std::cout << "wglGetProcAddress wglCreateContextAttribsARB" << std::endl;
        return false;
    }

    // 创建 opengl 3.3 的 rc，注意，这个创建必须在删除 temp_rc 之前
    // 因为 wglCreateContextAttribsARB 这个函数依赖 temp_rc 存在，如果删掉，下面的创建有问题
    const int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC morden_render_ctx = wglCreateContextAttribsARB(hdc, 0, attribs);
    if(!morden_render_ctx){
        std::cout << "morden_render_ctx" << std::endl;
        return false;
    }

    // 删除 temp rc
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(temp_rc);

    wglMakeCurrent(hdc, morden_render_ctx);

    // 初始化 glad，现在要求 rc 是 3.3+
    if (!gladLoaderLoadWGL(hdc)){
        std::cout << "gladLoaderLoadWGL" << std::endl;
        return false;
    }
    if (!gladLoaderLoadGL()){
        std::cout << "gladLoaderLoadGL" << std::endl;
        return false;
    }

    // 加载之后，这里应该能找到 glGetString 符号，不然就是加载有问题
    if (!glGetString){
        return false;
    }
    const char* version = (const char*)glGetString(GL_VERSION);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    // 这个 log 先留着后面改
    printf("Minimum OpenGL support:\nVersion: %s\nRenderer: %s\n", version, renderer);

    // opengl 的一些基础设置，先设置一下
    glClearColor(0.0, 0.0, 0.0, 1.0);

    return true;
}
#endif


static unsigned int CompileShader(const char* filename, GLenum shader_type) {
    // read file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "open file failed: " << filename << std::endl;
        return 0; // 文件打开失败
    }
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    std::string code_str = stream.str();
    const char* code = code_str.c_str();

    // compile shader
    unsigned int shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << filename << ", shader compile err: \n" << infoLog << std::endl;
        return 0;
    }
    return shader;
}

ShaderProgram::ShaderProgram(const char* vertex_shader_path, const char* fragment_shader_path){
    unsigned int vertex_shader = CompileShader(vertex_shader_path, GL_VERTEX_SHADER);
    unsigned int fragment_shader = CompileShader(fragment_shader_path, GL_FRAGMENT_SHADER);
    
    program_ = glCreateProgram();
    glAttachShader(program_, vertex_shader);
    glAttachShader(program_, fragment_shader);
    glLinkProgram(program_);

    int success;
    char infoLog[512];
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program_, 512, NULL, infoLog);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

ShaderProgram::~ShaderProgram(){
    glDeleteProgram(program_);
}

unsigned int ShaderProgram::ProgramID(){
    return program_;
}

void ShaderProgram::Use(){
    glUseProgram(program_); 
}

void ShaderProgram::UniformInt(const std::string &name, int value) const{
    glUniform1i(glGetUniformLocation(program_, name.c_str()), value); 
}
void ShaderProgram::UniformFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(program_, name.c_str()), value); 
}
void ShaderProgram::UniformFloat3(const std::string &name, float v1, float v2, float v3){
    glUniform3f(glGetUniformLocation(program_, name.c_str()), v1, v2, v3); 
}
void ShaderProgram::UniformFloatMat4Vec(const std::string &name, float* data, size_t mat_num, unsigned int transpose){
    glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), mat_num, transpose, data);
}

Texture2D::Texture2D(
        const char* path, 
        unsigned int tex_unit, 
        unsigned int target_fmt, 
        unsigned int org_fmt,
        bool flip_load){
    stbi_set_flip_vertically_on_load(flip_load);  
    tex_unit_ = tex_unit;
    
    glGenTextures(1, &texture_);
    glActiveTexture(GL_TEXTURE0 + tex_unit);
    glBindTexture(GL_TEXTURE_2D, texture_);

    unsigned char *data = stbi_load(path, &width_, &height_, &channel_, 0); 
    glTexImage2D(GL_TEXTURE_2D, 0, target_fmt, width_, height_, 0, org_fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void Texture2D::BindUnit(unsigned int tex_unit){
    tex_unit_ = tex_unit;
    glActiveTexture(GL_TEXTURE0 + tex_unit);
    glBindTexture(GL_TEXTURE_2D, texture_);
}

unsigned int Texture2D::CurrentUnit() const {
    return tex_unit_;
}

EBO::EBO(unsigned int* data, unsigned int data_size, unsigned int draw_type){
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, data, draw_type);
}
void EBO::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
}
void EBO::UnBind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
EBO::~EBO(){
    glDeleteBuffers(1, &ebo_);
}

VAO::VAO(){
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
}
void VAO::Bind() const {
    glBindVertexArray(vao_);
}
void VAO::UnBind() const {
    glBindVertexArray(0);
}
VAO::~VAO(){
    glDeleteVertexArrays(1, &vao_);
}

}