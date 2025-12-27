#include <fstream>
#include <sstream>

#include "stb_image.h"
#include <glad/gl.h>

#include "fg/graphics/opengl/gl.h"
#include "fg/utils/log.h"

namespace fg::graphics::opengl {

/* ========================= ShaderProgram ========================= */

static unsigned int CompileShader(const std::string& filename,
                                  GLenum shader_type) {
    // read file
    std::ifstream file(filename);
    CHECK(file.is_open()) << "Open file failed: " << filename;

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
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        CHECK(false) << filename << ", shader compile err: \n" << infoLog;
    }
    return shader;
}

ShaderProgram::ShaderProgram(const std::string& vertex_shader_path,
                             const std::string& fragment_shader_path) {
    Init(vertex_shader_path, fragment_shader_path);
}

bool ShaderProgram::Init(const std::string& vertex_shader_path,
                         const std::string& fragment_shader_path) {
    unsigned int vertex_shader =
      CompileShader(vertex_shader_path, GL_VERTEX_SHADER);
    unsigned int fragment_shader =
      CompileShader(fragment_shader_path, GL_FRAGMENT_SHADER);

    program_ = glCreateProgram();
    glAttachShader(program_, vertex_shader);
    glAttachShader(program_, fragment_shader);
    glLinkProgram(program_);

    int success;
    char infoLog[512];
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program_, 512, NULL, infoLog);
        CHECK(false) << infoLog;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return success;
}

/* ========================= Texture2D ========================= */

Texture2D::Texture2D(const std::string& path,
                     unsigned int tex_unit,
                     bool flip_load) {
    Init(path, tex_unit, flip_load);
}

static unsigned int GetOrgFmt(unsigned int channel) {
    switch (channel) {
        case 1:
            return GL_RED;
        case 2:
            return GL_RG;
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;
        default:
            return 0;
    }
}

bool Texture2D::Init(const std::string& path,
                     unsigned int tex_unit,
                     bool flip_load) {
    VLOG(6) << "Load Texture2D " << path;
    tex_unit_ = tex_unit;
    stbi_set_flip_vertically_on_load(flip_load);

    glGenTextures(1, &texture_);
    glActiveTexture(GL_TEXTURE0 + tex_unit);
    glBindTexture(GL_TEXTURE_2D, texture_);

    unsigned char* data =
      stbi_load(path.c_str(), &width_, &height_, &channel_, 0);
    CHECK(data) << "load texture " << path
                << " failed, reason: " << stbi_failure_reason();
    unsigned int fmt = GetOrgFmt(channel_);
    glTexImage2D(
      GL_TEXTURE_2D, 0, fmt, width_, height_, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return true;
}

void Texture2D::BindUnit(unsigned int tex_unit) {
    tex_unit_ = tex_unit;
    glActiveTexture(GL_TEXTURE0 + tex_unit);
    glBindTexture(GL_TEXTURE_2D, texture_);
}

/* ========================= EBO ========================= */

EBO::EBO(unsigned int* data, unsigned int data_size, unsigned int draw_type) {
    Init(data, data_size, draw_type);
}

// data size 是整个 buffer 的 size，比如 size * sizeof(unsigned int)
bool EBO::Init(unsigned int* data,
               unsigned int data_size,
               unsigned int draw_type) {
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, data, draw_type);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}

}  // namespace fg_gl
