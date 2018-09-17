#version 330 core

layout (location = 0) in vec3 vertex;
//layout (location = 1) in vec3 normal;
layout (location = 1) in vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragPos;
//out vec3 fragNormal;
out vec2 texCoords;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.0f);
    fragPos = vec3(modelMatrix * vec4(vertex, 1.0f)); //get world space position
    //fragNormal = vec3(modelMatrix * vec4(normal, 0.0f));
    texCoords = uv;
}