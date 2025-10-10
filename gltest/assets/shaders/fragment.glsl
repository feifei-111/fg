#version 330 core

struct Material{
   sampler2D diffuse;
   sampler2D specular;
   sampler2D emission;
   float shininess;
};
struct Light{
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoord;
in vec3 Norm;
in vec3 WorldPos;

uniform Material material;
uniform Light light;
uniform vec3 CameraPos;
uniform vec3 CameraFront;

out vec4 FragColor;

void main()
{
   // 光源到 position 的距离的平方 -> 点光源入射衰减
   float r_sq = dot(light.position - WorldPos, light.position - WorldPos);

   // ambient
   vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

   // diffuse
   vec3 lightDir = normalize(light.position - WorldPos);
   float diff_scale = max(dot(Norm, lightDir), 0.0);
   vec3 diffuse = diff_scale * light.diffuse * texture(material.diffuse, TexCoord).rgb;

   // specular
   vec3 view_direc = normalize(CameraPos - WorldPos);
   float spec_scale = pow(max(dot(Norm, normalize(lightDir + view_direc)), 0.0), material.shininess);
   vec3 specular = spec_scale * light.specular * texture(material.specular, TexCoord).rgb;
   

   vec3 emission = vec3(0.0);
   if (texture(material.specular, TexCoord).r == 0.0) {
      emission = texture(material.emission, TexCoord).rgb;
   }

   vec3 result = 3 * (ambient + diffuse + specular) / r_sq + emission * 0.5;
   FragColor = vec4(result, 1.0);

   if (dot(-view_direc, CameraFront) > 0.995 && dot(Norm, CameraFront) < 0){
      // FragColor = vec4(result, 0.0);
      discard;
   }
   else{
      FragColor = vec4(result, 1.0);
   }
}