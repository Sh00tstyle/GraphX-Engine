#version 330 core

layout (location = 0) in vec3 aVertex;

uniform mat4 modelMatrix;

void main() {
    gl_Position =  modelMatrix * vec4(aVertex, 1.0f);
}