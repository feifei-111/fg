#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 tex;
out vec2 TexCoord;

uniform mat4 trans;

void main()
{
   gl_Position = trans * vec4(Pos, 1.0);
   TexCoord = tex;
}