#version 330 core 

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;

out vec4 ourColor;

void main() {
    gl_Position = vec4(vertex, 1.0f);
    ourColor = vec4(color, 1.0f);
}