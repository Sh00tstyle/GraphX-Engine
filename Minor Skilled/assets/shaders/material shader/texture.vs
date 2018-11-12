#version 330 core

const int LIGHTAMOUNT = 10;

//define the struct in the vertex shader as well, as we need access to the position and direction
struct Light {
    vec4 position;
    vec4 direction;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    int type;

    float constant;
    float linear;
    float quadratic;
    float innerCutoff;
    float outerCutoff;
    
    vec2 padding;
};

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout (std140) uniform matricesBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 lightSpaceMatrix;
};

layout (std140) uniform positionsBlock {
    vec3 cameraPos;
    vec3 directionalLightPos;
};

layout(std140) uniform lightsBlock {
    Light lights[LIGHTAMOUNT];
};

uniform mat4 modelMatrix;

out VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
    
    vec3 tangentLightPos[LIGHTAMOUNT];
    vec3 tangentLightDir[LIGHTAMOUNT];

    vec3 tangentViewPos;
    vec3 tangentFragPos;
    vec3 tangentFragNormal; //needed in case there is no normal map

    vec4 lightSpaceFragPos;
} vs_out;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix))); //fix normals in non uniform scaling

    vs_out.fragPos = vec3(modelMatrix * vec4(aVertex, 1.0f));
    vs_out.fragNormal = normalMatrix * aNormal;
    vs_out.texCoord = aUV;

    //construct TBN matrix
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T); //re-orthogonalize with gram-schmidt process
    mat3 TBN = transpose(mat3(T, B, N));

    //calculate tangent space positions (convert to world space first if needed)
    for(int i = 0; i < LIGHTAMOUNT; i++) {
        vs_out.tangentLightPos[i] = TBN * lights[i].position.xyz;
        vs_out.tangentLightDir[i] = TBN * lights[i].direction.xyz;
    }

    vs_out.tangentViewPos = TBN * cameraPos;
    vs_out.tangentFragPos = TBN * vs_out.fragPos;
    vs_out.tangentFragNormal = TBN * normalMatrix * aNormal;

    vs_out.lightSpaceFragPos =  lightSpaceMatrix * modelMatrix * vec4(aVertex, 1.0f);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}