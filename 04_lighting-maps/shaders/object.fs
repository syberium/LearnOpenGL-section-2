#version 330 core
struct Material {
   sampler2D diffuse;
   sampler2D specular;
   sampler2D emission;
   float shininess;
};

struct Light {
   vec3 position;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};
// imput parameters (from fs)
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

// uniform parameters (is set in main)
uniform Material material;
uniform Light light;
uniform vec3 viewPos;

uniform float textShift;
uniform float textGlow;

// out parameter
out vec4 FragColor;

void main()
{
   // ambient light part
   vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
   
   // diffuse light part
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(light.position - FragPos);
   float diff = max(dot(norm, lightDir), 0.0);

   vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

   // specular light part
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

   vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

   vec3 emission = vec3(0.0);
   if (texture(material.specular, TexCoords).r == 0.0)
   {
      emission = texture(material.emission, TexCoords + vec2(0.0, textShift)).rgb;
      emission = emission * textGlow;
   }
   
   // result
   vec3 result = ambient + diffuse + specular + emission;
   FragColor = vec4(result, 1.0);
}
