#version 330 core

//material struct
struct Material {
    sampler2D diffuse;
    sampler2D specular;
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
in vec2 texCoords;

uniform vec3 cameraPos;

uniform Material material;
uniform Light light;

out vec4 fragColor;

void main() {
    //calculate ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));

    //calculate diffuse
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(light.position - fragPos); //direction from fragment to light
    float difference = max(dot(normal, lightDirection), 0.0f); //cosine of the angle between the normal and the light direction (dot)
    vec3 diffuse = light.diffuse * difference * vec3(texture(material.diffuse, texCoords));

    //calculate specular
    vec3 lookDirection = normalize(cameraPos - fragPos); //direction from fragment to camera
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularity = pow(max(dot(lookDirection, reflectDirection), 0.0f), material.shininess);
    vec3 specular = light.specular * specularity * vec3(texture(material.specular, texCoords));

    fragColor = vec4(ambient + diffuse + specular, 1.0f);
}