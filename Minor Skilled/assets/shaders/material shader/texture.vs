#version 330 core

const int LIGHTAMOUNT = 10;

struct Light { //define the struct here as well, as we need access to the position and direction
    int type;

    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
    float innerCutoff;
    float outerCutoff;
};

layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform Light lights[LIGHTAMOUNT];

uniform vec3 cameraPos;

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
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix))); //fix normals non uniform scaling
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T); //re-orthogonalize with gram-schmidt process
    mat3 TBN = transpose(mat3(T, B, N));

    //calculate tangent space positions (convert to world space first if needed)
    for(int i = 0; i < LIGHTAMOUNT; i++) {
        vs_out.tangentLightPos[i] = TBN * lights[i].position;
        vs_out.tangentLightDir[i] = TBN * lights[i].direction;
    }

    vs_out.tangentViewPos = TBN * cameraPos;
    vs_out.tangentFragPos = TBN * vec3(modelMatrix * vec4(aVertex, 1.0f));
    vs_out.tangentFragNormal = TBN * normalMatrix * aNormal;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}