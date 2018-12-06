#version 460 core

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
} vs_out;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    vs_out.fragPos = vec3(modelMatrix * vec4(aVertex, 1.0f));
    vs_out.fragNormal = normalMatrix * aNormal;
    vs_out.texCoord = aUV;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}