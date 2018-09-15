#version 330 core

//material struct
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

//light struct
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

 //lighting
in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 cameraPos;

uniform Material material;
uniform Light light;

out vec4 fragColor;

void main() {
    //calculate ambient
    vec3 ambient = light.ambient * material.ambient;

    //calculate diffuse
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(light.position - fragPos); //direction from fragment to light
    float difference = max(dot(normal, lightDirection), 0.0f); //cosine of the angle between the normal and the light direction (dot)
    vec3 diffuse = light.diffuse * (difference * material.diffuse);

    //calculate specular
    vec3 lookDirection = normalize(cameraPos - fragPos); //direction from fragment to camera
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularity = pow(max(dot(lookDirection, reflectDirection), 0.0f), material.shininess);
    vec3 specular = light.specular * (specularity * material.specular);

    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0f);
}