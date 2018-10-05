#version 330 core

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 texCoord;
out vec3 fragPos;
out vec3 fragNormal;

void main() {
    texCoord = aUV;
    fragPos = vec3(modelMatrix * vec4(aVertex, 1.0f));
    fragNormal = mat3(modelMatrix) * aNormal;   

    gl_Position =  projectionMatrix * viewMatrix * vec4(fragPos, 1.0f);
}