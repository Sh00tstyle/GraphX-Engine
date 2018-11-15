#version 460 core

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

layout (std140) uniform matricesBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 lightSpaceMatrix;
};

layout (std140) uniform dataBlock {
    bool useShadows;

    vec3 cameraPos;
    vec3 directionalLightPos;
};

layout(std430) buffer lightsBlock {
    int usedLights;
    Light lights[];
};

uniform mat4 modelMatrix;

out VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;

    mat3 TBN;

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
    T = normalize(T - dot(T, N) * N); //re-orthogonalize with gram-schmidt process
    vec3 B = cross(N, T); 
    vs_out.TBN = mat3(T, B, N);

    vs_out.lightSpaceFragPos =  lightSpaceMatrix * modelMatrix * vec4(aVertex, 1.0f);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
}