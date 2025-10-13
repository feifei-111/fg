#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

out vec2 TexCoord;
out vec3 Norm;
out vec3 WorldPos;

uniform mat3 normal;
uniform mat4 model;
uniform mat4 p_v;

void main()
{
   TexCoord = tex;
   Norm = normalize(normal * norm);
   // 为了在 world space 计算 lighting，还需要 space world 的坐标
   WorldPos = vec3(model * vec4(pos, 1.0));
   gl_Position = p_v * vec4(WorldPos, 1.0);
}