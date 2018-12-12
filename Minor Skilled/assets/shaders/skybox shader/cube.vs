#version 460 core

layout (location = 0) in vec3 aVertex;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragPos;

void main() {
    fragPos = aVertex;

    gl_Position = projectionMatrix * viewMatrix * vec4(aVertex, 1.0f);
}