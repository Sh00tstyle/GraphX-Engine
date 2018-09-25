#version 330 core

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform bool reverseNormals;

out VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
} vs_out;

void main() {
    vs_out.fragPos = vec3(modelMatrix * vec4(aVertex, 1.0f)); //transform to world space
    vs_out.texCoord = aUV;

    if(reverseNormals) {
        vs_out.fragNormal = transpose(inverse(mat3(modelMatrix))) * (-1.0f * aNormal);
    } else {
        vs_out.fragNormal = transpose(inverse(mat3(modelMatrix))) * aNormal; 
    }

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}