#version 460 core

//array of offset directions for sampling
vec3 gridSamplingDisk[20] = vec3[] (
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

//light types
const int DIRECTIONAL = 0;

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

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;

    vec4 lightSpaceFragPos;
} fs_in;

layout (std140) uniform dataBlock {
    bool useShadows;
    int usedCubeShadows;
    float farPlane;

    vec3 cameraPos;
    vec3 directionalLightPos;

    vec3 pointLightPositions[5];
};

layout(std430) buffer lightsBlock {
    int usedLights;
    Light lights[];
};

uniform vec3 renderPos;

uniform bool useTexture;
uniform vec3 diffuseVec;
uniform sampler2D diffuseMap;

uniform sampler2D shadowMap;

out vec4 fragColor;

vec3 CalculateDirectionalLight(Light light, vec3 diff, vec3 normal, vec3 viewDirection, float shadow);
float CalculateShadow(vec3 normal);

void main() {
    vec3 normal = normalize(fs_in.fragNormal);

    //diffuse color
    vec3 diffuse;

    if(useTexture) diffuse = texture(diffuseMap, fs_in.texCoord).rgb;
    else diffuse = diffuseVec;

    //shadow
    float shadow = CalculateShadow(normal);
    shadow = 1.0f - shadow;

    //directional light
    vec3 viewDirection = normalize(renderPos - fs_in.fragPos);
    vec3 result = vec3(0.0f);

    for(int i = 0; i < usedLights; i++) {
        if(lights[i].type != DIRECTIONAL) continue;

        result += CalculateDirectionalLight(lights[i], diffuse, normal, viewDirection, shadow);
    }

    fragColor = vec4(result, 1.0f);
}

vec3 CalculateDirectionalLight(Light light, vec3 diff, vec3 normal, vec3 viewDirection, float shadow) {
    //ambient
    vec3 ambient = light.ambient.rgb * diff;

    //diffuse
    float difference = max(dot(normal, -light.direction.xyz), 0.0f);
    vec3 diffuse = light.diffuse.rgb * difference * diff;

    //specular
    vec3 halfwayDireciton = normalize(light.direction.xyz + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), 32.0f); //hardcoded shininess of 32
    vec3 specular = light.specular.rgb * specularity * 0.2f; //hardcoded specular contribution of 0.2

    //combine results
    return (ambient + shadow * (diffuse + specular));
}

float CalculateShadow(vec3 normal) {
    if(!useShadows) return 0.0f; //no shadows

    //perform perspective divide
    vec3 projectedCoords = fs_in.lightSpaceFragPos.xyz / fs_in.lightSpaceFragPos.w;

    //transform to [0,1] range
    projectedCoords = projectedCoords * 0.5f + 0.5f;

    //get closest depth value from lights perspective (using [0,1] range lightSpaceFragPos as coords)
    float closestDepth = texture(shadowMap, projectedCoords.xy).r; 

    //get depth of current fragment from lights perspective
    float currentDepth = projectedCoords.z;

    //calculate bias based on depth map resolution and slope
    vec3 lightDirection = normalize(directionalLightPos - fs_in.fragPos);
    float bias = max(0.15f * (1.0f - dot(normal, lightDirection)), 0.015f);

    //PCF
    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projectedCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;        
        }    
    }

    shadow /= 9.0f;
    
    //keep the shadow at 0.0f when outside the far plane region of the lights frustum.
    if(projectedCoords.z > 1.0f) shadow = 0.0f;

    return shadow;
}