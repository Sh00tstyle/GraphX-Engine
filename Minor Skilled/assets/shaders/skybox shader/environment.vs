#version 460 core

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

layout (std140) uniform matricesBlock {
    mat4 viewMatrix; //should not be used here
    mat4 projectionMatrix; //should not be used here
    mat4 lightSpaceMatrix; 
};

uniform mat4 modelMatrix;
uniform mat4 viewMatrixCube;
uniform mat4 projectionMatrixCube;

out VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;

    vec4 lightSpaceFragPos;
} vs_out;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    vs_out.fragPos = vec3(modelMatrix * vec4(aVertex, 1.0f));
    vs_out.fragNormal = normalMatrix * aNormal;
    vs_out.texCoord = aUV;

    vs_out.lightSpaceFragPos = lightSpaceMatrix * modelMatrix * vec4(aVertex, 1.0f);

    gl_Position = projectionMatrixCube * viewMatrixCube * modelMatrix * vec4(aVertex, 1.0f);
}