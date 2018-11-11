#version 330 core

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 lightSpaceMatrix;

out VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;

    vec4 lightSpaceFragPos;
} vs_out;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix))); //fix normals non uniform scaling

    vs_out.fragPos = vec3(modelMatrix * vec4(aVertex, 1.0f));
    vs_out.fragNormal = normalMatrix * aNormal; //Note: the normal matrix has no translation

    vs_out.lightSpaceFragPos = lightSpaceMatrix * modelMatrix * vec4(aVertex, 1.0f);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}