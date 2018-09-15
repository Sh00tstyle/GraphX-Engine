#version 330 core

 //lighting
in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 lightPos;
uniform vec3 cameraPos;

uniform vec3 objectColor;
uniform vec3 lightColor;

out vec4 fragColor;

void main() {
    //calculate ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    //calculate diffuse
    vec3 normal = normalize(fragNormal);
    vec3 lightDirection = normalize(lightPos - fragPos); //direction from fragment to light
    float difference = max(dot(normal, lightDirection), 0.0f); //cosine of the angle between the normal and the light direction (dot)
    vec3 diffuse = difference * lightColor;

    //calculate specular
    float specularStrength = 0.5f;
    vec3 lookDirection = normalize(cameraPos - fragPos); //direction from fragment to camera
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularity = pow(max(dot(lookDirection, reflectDirection), 0.0f), 32); //32 is the shininess
    vec3 specular = specularStrength * specularity * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(result, 1.0f);
}