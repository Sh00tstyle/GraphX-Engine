#version 330 core

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
//layout (location = 2) in vec2 uv; //needs to be 2 for the model, 1 is skipped since it doesnt exist

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

//interface block
out VS_OUT {
    //vec2 texCoords;
    vec3 normal;
} vs_out;

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(viewMatrix * modelMatrix)));
    vs_out.normal = normalize(vec3(projectionMatrix * vec4(normalMatrix * aNormal, 0.0f))); //transform to clip space for scale and rotation

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}