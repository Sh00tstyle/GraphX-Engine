#version 330 core

layout (location = 0) in vec3 aVertex;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 texCoord;

void main() {
    texCoord = aVertex;

    vec4 position = projectionMatrix * viewMatrix * vec4(aVertex, 1.0f);
    gl_Position = position.xyww; //enforce depth value to be 1.0f at all times
}