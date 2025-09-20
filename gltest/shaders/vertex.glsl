#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec2 tex;
out vec2 TexCoord;
uniform vec3 offset;

void main()
{
   gl_Position = vec4(Pos + offset, 1.0);
   TexCoord = tex;
}