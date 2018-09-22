#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

layout (std140) uniform Matrices {
    mat4 projectionMatrix;
    mat4 viewMatrix;
};

uniform mat4 modelMatrix;

out vec3 fragPos;
out vec3 fragNormal;

void main() {
    fragNormal = mat3(transpose(inverse(modelMatrix))) * normal;
    fragPos = vec3(modelMatrix * vec4(vertex, 1.0f));

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.0f);
}