#version 330 core

in vec3 fragPos;
in vec3 fragNormal;

uniform vec3 cameraPos;
uniform samplerCube cubemap;

out vec4 fragColor;

void main() {
    //reflection
    //vec3 lookDirection = normalize(fragPos - cameraPos);
    //vec3 reflection = reflect(lookDirection, normalize(fragNormal));

    //fragColor = vec4(texture(cubemap, reflection).rgb, 1.0f);

    //refraction
    float ratio = 1.0f / 1.52f;
    vec3 lookDirection = normalize(fragPos - cameraPos);
    vec3 refraction = refract(lookDirection, normalize(fragNormal), ratio);

    fragColor = vec4(texture(cubemap, refraction).rgb, 1.0f);
}