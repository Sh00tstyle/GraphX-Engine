#version 330 core

layout (location = 0) in vec2 aVertex;
layout (location = 1) in vec2 aUV;

out vec2 texCoord;

void main() {
    gl_Position = vec4(aVertex.x, aVertex.y, 0.0f, 1.0f);
    texCoord = aUV;
}