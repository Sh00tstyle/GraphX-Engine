#version 460 core

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;

layout (std140) uniform matricesBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 previousViewProjectionMatrix;
    mat4 lightSpaceMatrix;
};

uniform mat4 modelMatrix;

out VS_OUT {
    vec3 fragPosWorld;
    vec3 fragPosView;
    vec3 fragNormal;
    vec2 texCoord;

    mat3 TBN;

    vec4 lightSpaceFragPos;
} vs_out;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix))); //fix normals in non uniform scaling

    vs_out.fragPosWorld = vec3(modelMatrix * vec4(aVertex, 1.0f));
    vs_out.fragPosView = vec3(viewMatrix * modelMatrix * vec4(aVertex, 1.0f));
    vs_out.fragNormal = normalMatrix * aNormal;
    vs_out.texCoord = aUV;

    //construct TBN matrix
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N); //re-orthogonalize with gram-schmidt process
    vec3 B = cross(N, T); 
    vs_out.TBN = mat3(T, B, N);

    vs_out.lightSpaceFragPos = lightSpaceMatrix * modelMatrix * vec4(aVertex, 1.0f);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}