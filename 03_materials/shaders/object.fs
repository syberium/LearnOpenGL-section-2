#version 330 core
struct Material {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
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

// uniform parameters (is set in main)
uniform Material material;
uniform Light light;

uniform vec3 viewPos;

// out parameter
out vec4 FragColor;

void main()
{
   // ambient light part
   vec3 ambient = light.ambient * material.ambient;
   
   // diffuse light part
   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(light.position - FragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = light.diffuse * (diff * material.diffuse);

   // specular light part
   vec3 viewDir = normalize(viewPos - FragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   vec3 specular = light.specular * (spec * material.specular);

   vec3 result = ambient + diffuse + specular;
   FragColor = vec4(result, 1.0);
}
