#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D Tex0;
uniform sampler2D Tex1;

void main()
{
   FragColor = mix(texture(Tex0, TexCoord * 2), texture(Tex1, TexCoord), 0.8);
}