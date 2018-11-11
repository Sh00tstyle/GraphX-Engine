#version 330 core

layout (location = 0) in vec3 aVertex;

uniform mat4 lightSpaceMatrix; //light projection * light view (precalculated)
uniform mat4 modelMatrix;

void main() {
    gl_Position = lightSpaceMatrix * modelMatrix * vec4(aVertex, 1.0f);
}