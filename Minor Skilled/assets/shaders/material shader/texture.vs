#version 330 core

const int LIGHTAMOUNT = 10; //TODO: replace with uniform buffer objects

//define the struct in the vertex shader as well, as we need access to the position and direction
struct VertLight { 
    vec3 position; //world space
    vec3 direction;
};

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform VertLight vertLights[LIGHTAMOUNT];
uniform vec3 cameraPos; //world space

out VS_OUT {
    vec2 texCoord;
    
    vec3 tangentLightPos[LIGHTAMOUNT];
    vec3 tangentLightDir[LIGHTAMOUNT];

    vec3 tangentViewPos;
    vec3 tangentFragPos;
    vec3 tangentFragNormal; //needed in case there is no normal map
} vs_out;

void main() {
    vs_out.texCoord = aUV;

    //construct TBN matrix
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix))); //fix normals in non uniform scaling
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T); //re-orthogonalize with gram-schmidt process
    mat3 TBN = transpose(mat3(T, B, N));

    //calculate tangent space positions (convert to world space first if needed)
    for(int i = 0; i < LIGHTAMOUNT; i++) {
        vs_out.tangentLightPos[i] = TBN * vertLights[i].position;
        vs_out.tangentLightDir[i] = TBN * vertLights[i].direction;
    }

    vs_out.tangentViewPos = TBN * cameraPos;
    vs_out.tangentFragPos = TBN * vec3(modelMatrix * vec4(aVertex, 1.0f));
    vs_out.tangentFragNormal = TBN * normalMatrix * aNormal;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}