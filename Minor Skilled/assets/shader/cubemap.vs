#version 330 core

layout (location = 0) in vec3 vertex;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 textureDir;

void main() {
    vec4 pos = projectionMatrix * viewMatrix * vec4(vertex, 1.0);
    gl_Position = pos.xyww;

    textureDir = vertex;
}