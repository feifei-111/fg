#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexDiffuse0;

void main()
{    
    FragColor = texture(TexDiffuse0, TexCoords);
}