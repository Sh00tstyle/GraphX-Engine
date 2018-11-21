#version 460 core

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

layout (std140) uniform matricesBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 lightSpaceMatrix;
};

uniform mat4 modelMatrix;

out VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
} vs_out;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix))); //fix normals non uniform scaling

    vs_out.fragPos = vec3(modelMatrix * vec4(aVertex, 1.0f));
    vs_out.fragNormal = normalMatrix * aNormal; //Note: the normal matrix has no translation

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}