#version 330 core

layout (location = 0) in vec3 aVertex;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 textureDir;

void main() {
    textureDir = aVertex;
    gl_Position = projectionMatrix * viewMatrix * vec4(aVertex, 1.0f);
}