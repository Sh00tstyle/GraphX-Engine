#version 330 core 

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 uv;

out vec4 ourColor;
out vec2 texCoord;

void main() {
    gl_Position = vec4(vertex, 1.0f);
    ourColor = vec4(color, 1.0f);
    texCoord = uv;
}