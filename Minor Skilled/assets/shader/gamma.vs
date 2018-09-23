#version 330 core

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out VS_OUT { //output interface block
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
} vs_out;

void main() {
    gl_Position = projectionMatrix * viewMatrix * vec4(aVertex, 1.0f);
    vs_out.fragPos = aVertex;
    vs_out.fragNormal = aNormal;
    vs_out.texCoord = aUV;
}