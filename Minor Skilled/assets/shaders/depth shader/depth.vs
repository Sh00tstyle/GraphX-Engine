#version 460 core

layout (location = 0) in vec3 aVertex;

layout (std140) uniform matricesBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 previousViewProjectionMatrix;
    mat4 lightSpaceMatrix;
};

uniform mat4 modelMatrix;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}