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
uniform float time;

out vec4 FragColor;

vec3 get_color(vec3 light_pos){
   vec3 light_dir = normalize(light_pos - WorldPos);
   vec3 view_dir = normalize(CameraPos - WorldPos);

   // attenuation
   float dist = length(light_pos - WorldPos);
   float attenuation = 1 / (1.0 + 0.07 * dist + 0.017 * dist * dist);

   // ambient
   vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

   // diffuse
   float diff_scale = max(dot(Norm, light_dir), 0.0);
   vec3 diffuse = diff_scale * light.diffuse * texture(material.diffuse, TexCoord).rgb;

   // specular
   float spec_scale = pow(max(dot(Norm, normalize(light_dir + view_dir)), 0.0), material.shininess);
   vec3 specular = spec_scale * light.specular * texture(material.specular, TexCoord).rgb;

   vec3 result = (ambient + diffuse + specular) * attenuation;
   return result;
}

vec3 get_spot_light_color(vec3 light_pos, vec3 light_center){
   vec3 light_dir = normalize(light_pos - WorldPos);
   vec3 view_dir = normalize(CameraPos - WorldPos);

   float theta = dot(-view_dir, light_center);

   // attenuation
   float dist = length(light_pos - WorldPos);
   float attenuation = 1 / (1.0 + 0.07 * dist + 0.017 * dist * dist);

   // ambient
   vec3 ambient = light.ambient * texture(material.diffuse, TexCoord).rgb;

   // diffuse
   float diff_scale = max(dot(Norm, light_dir), 0.0);
   vec3 diffuse = diff_scale * light.diffuse * texture(material.diffuse, TexCoord).rgb;

   // specular
   float spec_scale = pow(max(dot(Norm, normalize(light_dir + view_dir)), 0.0), material.shininess);
   vec3 specular = spec_scale * light.specular * texture(material.specular, TexCoord).rgb;

   float outer_cutoff = 0.98;
   float cutoff = 0.99;
   float spot_light_scale;

   spot_light_scale = clamp((theta - outer_cutoff) / (cutoff - outer_cutoff), 0.0, 1.0);

   vec3 result = (ambient + (diffuse + specular) * spot_light_scale) * attenuation;
   return result;
}

void main()
{
   vec3 spot = get_spot_light_color(CameraPos, CameraFront);
   vec3 point = get_color(light.position);

   // emission
   vec3 emission = vec3(0.0);
   if (texture(material.specular, TexCoord).r == 0.0) {
      emission = texture(material.emission, TexCoord).rgb * 0.5;
   }

   vec3 result = spot + point + emission;
   FragColor = vec4(result, 1.0);
}