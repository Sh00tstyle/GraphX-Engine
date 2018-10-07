#version 330 core

layout (location = 0) in vec3 aVertex;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 textureDir;

void main() {
    textureDir = aVertex;

    mat4 rotView = mat4(mat3(viewMatrix));
    vec4 clipPos = projectionMatrix * rotView * vec4(aVertex, 1.0f);

    gl_Position = clipPos.xyww;
}