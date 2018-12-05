#version 460 core

layout (location = 0) in vec3 aVertex;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 fragPos;

void main() {
    fragPos = vec3(modelMatrix * vec4(aVertex, 1.0f));
    
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}