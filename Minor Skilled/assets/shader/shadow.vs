#version 330 core

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 lightSpaceMatrix; //light projection * view matrices

out VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
    vec4 fragPosLightSpace;
} vs_out;

void main() {
    vs_out.fragPos = vec3(modelMatrix * vec4(aVertex, 1.0f)); //transform to world space
    vs_out.fragNormal = transpose(inverse(mat3(modelMatrix))) * aNormal; //remove translation from modelMatrix and transform to world space (and avoid non uniform normal scaling issues)
    vs_out.texCoord = aUV;
    vs_out.fragPosLightSpace = lightSpaceMatrix * vec4(vs_out.fragPos, 1.0f); //transform to light space (similar to clip space, just from the lights pov)

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}