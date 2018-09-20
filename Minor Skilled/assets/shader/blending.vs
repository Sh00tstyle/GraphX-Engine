#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragPos;
out vec3 fragNormal;

void main() {
    fragNormal = mat3(transpose(inverse(modelMatrix))) * normal;
    fragPos = vec3(modelMatrix * vec4(vertex, 1.0f));

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.0f);
}