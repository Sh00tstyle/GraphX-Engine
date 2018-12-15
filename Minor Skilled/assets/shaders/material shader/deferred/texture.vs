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
    vec3 fragNormalView;
    vec3 fragNormalWorld;
    vec2 texCoord;

    mat3 TBN;
} vs_out;

void main() {
    mat3 normalMatrixView = transpose(inverse(mat3(viewMatrix * modelMatrix))); //fix normals in non uniform scaling
    mat3 normalMatrixWorld = transpose(inverse(mat3(modelMatrix)));

    vs_out.fragPosWorld = vec3(modelMatrix * vec4(aVertex, 1.0f)); //world space
    vs_out.fragPosView = vec3(viewMatrix * modelMatrix * vec4(aVertex, 1.0f)); //view space
    vs_out.fragNormalView = normalMatrixView * aNormal; //view space
    vs_out.fragNormalWorld = normalMatrixWorld * aNormal; //world space
    vs_out.texCoord = aUV;

    //construct TBN matrix (transforms from tangent to view space)
    vec3 T = normalize(normalMatrixView * aTangent);
    vec3 N = normalize(normalMatrixView * aNormal);
    T = normalize(T - dot(T, N) * N); //re-orthogonalize with gram-schmidt process
    vec3 B = cross(N, T); 
    vs_out.TBN = mat3(T, B, N);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}