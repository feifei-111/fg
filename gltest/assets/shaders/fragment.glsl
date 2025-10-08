#version 330 core

in vec2 TexCoord;
in vec3 Norm;
in vec3 WorldPos;

out vec4 FragColor;

uniform sampler2D Tex0;
uniform sampler2D Tex1;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform vec3 ObjColor;
uniform vec3 CameraPos;

void main()
{
   // FragColor = mix(texture(Tex0, TexCoord * 2), texture(Tex1, TexCoord), 0.8);
   
   vec3 ambient = 0.1 * LightColor;

   // 表面指向光源
   vec3 in_light_unnorm = LightPos - WorldPos;

   vec3 in_light = normalize(LightPos - WorldPos);
   vec3 diffuse = max(dot(Norm, in_light), 0.0) * LightColor;

   vec3 view_direc = normalize(CameraPos - WorldPos);

   // 这个是近似算法，效果好像差不多
   vec3 specular = pow(max(dot(Norm, normalize(in_light + view_direc)), 0.0), 256) * LightColor;
   // vec3 specular = 3 * pow(max(dot(view_direc, reflect(-in_light, Norm)), 0.0), 256) * LightColor;

   // 只算了入射衰减
   FragColor = 2 * vec4(((ambient + diffuse + specular) * ObjColor), 1.0) / dot(in_light_unnorm, in_light_unnorm);
}