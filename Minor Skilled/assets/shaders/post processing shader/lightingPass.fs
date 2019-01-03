#version 460 core

//light types
const int DIRECTIONAL = 0;
const int POINT = 1;
const int SPOT = 2;

//array of offset directions for sampling
vec3 gridSamplingDisk[20] = vec3[] (
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

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

in vec2 texCoord;

layout (std140) uniform matricesBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 previousViewProjectionMatrix;
    mat4 lightSpaceMatrix;
};

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

uniform bool useSSAO;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gEmissionSpec;
uniform sampler2D gEnvironmentShiny;

uniform sampler2D ssao;
uniform samplerCube environmentMap;
uniform sampler2D shadowMap;
uniform samplerCube shadowCubemaps[5];

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec3 brightColor;

vec3 CalculateDirectionalLight(Light light, vec3 albedo, float spec, float shininess, vec3 normal, vec3 viewDirection, vec2 texCoord, float shadow);
vec3 CalculatePointLight(Light light, vec3 albedo, float spec, float shininess, vec3 normal, vec3 fragPos, vec3 viewDirection, vec2 texCoord, float shadow);
vec3 CalculateSpotLight(Light light, vec3 albedo, float spec, float shininess, vec3 normal, vec3 fragPos, vec3 viewDirection, vec2 texCoord, float shadow);

float CalculateShadow(vec3 normal, vec3 fragPos, vec4 lightSpaceFragPos);
float CalculateCubemapShadow(vec3 normal, vec3 fragPos, int index);

vec3 CalculateBrightColor(vec3 color);

void main() {
    //sample data from the gBuffer textures
    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 normal = texture(gNormal, texCoord).rgb;
    vec3 albedo = texture(gAlbedo, texCoord).rgb;
    vec3 environment = texture(gEnvironmentShiny, texCoord).rgb;
    float specular = texture(gEmissionSpec, texCoord).g;
    float shininess = texture(gEnvironmentShiny, texCoord).a * 255.0f;

    //transform to world pos
    vec3 worldFragPos = vec3(inverse(viewMatrix) * vec4(fragPos, 1.0f));
    vec3 worldNormal = vec3(inverse(viewMatrix) * vec4(normal, 0.0f));

    //reflection
    if(length(environment) > 0.0f) {
        //only output the reflection and skip everything else if there is reflection
        fragColor = vec4(environment, 1.0f);
        brightColor = vec3(0.0f);
        return;
    }

    //shadows
    vec4 lightSpaceFragPos = lightSpaceMatrix * vec4(worldFragPos, 1.0f);

    float shadow = CalculateShadow(worldNormal, worldFragPos, lightSpaceFragPos);

    for(int i = 0; i < usedCubeShadows; i++) {
        shadow += CalculateCubemapShadow(worldNormal, worldFragPos, i);
    }

    if(shadow > 1.0f) shadow = 1.0f;
    shadow = 1.0f - shadow;

    //lighting
    vec3 viewDirection = normalize(cameraPos - worldFragPos);
    vec3 result = vec3(0.0f);

    for(int i = 0; i < usedLights; i++) {
        switch(lights[i].type) {
            case DIRECTIONAL:
                result += CalculateDirectionalLight(lights[i], albedo, specular, shininess, worldNormal, viewDirection, texCoord, shadow);
                break;

            case POINT:
                result += CalculatePointLight(lights[i], albedo, specular, shininess, worldNormal, worldFragPos, viewDirection, texCoord, shadow);
                break;

            case SPOT:
                result += CalculateSpotLight(lights[i], albedo, specular, shininess, worldNormal, worldFragPos, viewDirection, texCoord, shadow);
                break;
        }
    }

    if(usedLights == 0) { //in case we have no light, simply take the albedo
        result = albedo;
    }

    //emission
    vec3 emission = albedo * texture(gEmissionSpec, texCoord).r;
    result += emission;

    //output bright color before applying AO
    brightColor.rgb = CalculateBrightColor(result);

    //ambient occlusion
    if(useSSAO) {
        float ao = texture(ssao, texCoord).r;
        result *= ao;
    }

    //output normal frag color with AO
    fragColor = vec4(result, 1.0f);
}

vec3 CalculateDirectionalLight(Light light, vec3 albedo, float spec, float shininess, vec3 normal, vec3 viewDirection, vec2 texCoord, float shadow) {
    //ambient
    vec3 ambient = light.ambient.rgb * albedo;

    //diffuse
    float difference = max(dot(normal, -light.direction.xyz), 0.0f);
    vec3 diffuse = light.diffuse.rgb * difference * albedo;

    //specular
    vec3 halfwayDireciton = normalize(light.direction.xyz + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), shininess);
    vec3 specular = light.specular.rgb * specularity * spec;

    //combine results
    return (ambient + shadow * (diffuse + specular));
}

vec3 CalculatePointLight(Light light, vec3 albedo, float spec, float shininess, vec3 normal, vec3 fragPos, vec3 viewDirection, vec2 texCoord, float shadow) {
    vec3 lightDirection = normalize(light.position.xyz - fragPos);

    //ambient
    vec3 ambient = light.ambient.rgb * albedo;

    //diffuse
    float difference = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = light.diffuse.rgb * difference * albedo;

    //specular
    vec3 halfwayDireciton = normalize(lightDirection + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), shininess);
    vec3 specular = light.specular.rgb * specularity * spec;

    //attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //combine results
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + shadow * (diffuse + specular));
}

vec3 CalculateSpotLight(Light light, vec3 albedo, float spec, float shininess, vec3 normal, vec3 fragPos, vec3 viewDirection, vec2 texCoord, float shadow) {
    vec3 lightDirection = normalize(light.position.xyz - fragPos);

    //ambient
    vec3 ambient = light.ambient.rgb * albedo;

    //diffuse
    float difference = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = light.diffuse.rgb * difference * albedo;

    //specular
    vec3 halfwayDireciton = normalize(lightDirection + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), shininess);
    vec3 specular = light.specular.rgb * specularity * spec;

    //attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //spotlight
    float theta = dot(lightDirection, normalize(light.direction.xyz));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

    //combine results
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + shadow * (diffuse + specular));
}

float CalculateShadow(vec3 normal, vec3 fragPos, vec4 lightSpaceFragPos) {
    if(!useShadows) return 0.0f; //no shadows

    //perform perspective divide
    vec3 projectedCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;

    //transform to [0,1] range
    projectedCoords = projectedCoords * 0.5f + 0.5f;

    //get closest depth value from lights perspective (using [0,1] range lightSpaceFragPos as coords)
    float closestDepth = texture(shadowMap, projectedCoords.xy).r; 

    //get depth of current fragment from lights perspective
    float currentDepth = projectedCoords.z;

    //calculate bias based on depth map resolution and slope
    vec3 lightDirection = normalize(directionalLightPos - fragPos);
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

float CalculateCubemapShadow(vec3 normal, vec3 fragPos, int index) {
    if(!useShadows) return 0.0f; //no shadows

    vec3 lightPos = pointLightPositions[index];

    //get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;

    //now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);

    //calculate bias based on depth map resolution and slope
    vec3 lightDirection = normalize(lightPos - fragPos);
    float bias = max(0.25f * (1.0f - dot(normal, lightDirection)), 0.1f);

    //test for shadows and apply PCF
    float shadow = 0.0f;
    int samples = 20;
    float viewDistance = length(cameraPos - fragPos);
    float diskRadius = (1.0f + (viewDistance / farPlane)) / 22.0f;

    float weight = 1.0f - (length(lightDirection) / farPlane * 0.001f);
    bias *= weight;

    for(int i = 0; i < samples; i++) {
        float closestDepth = texture(shadowCubemaps[index], fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= farPlane; //undo mapping [0, 1]

        if(currentDepth - bias > closestDepth) shadow += 1.0f;
    }

    shadow /= float(samples);
        
    return shadow;
}

vec3 CalculateBrightColor(vec3 color) {
    const vec3 threshold = vec3(0.2126f, 0.7152f, 0.0722f);

    float brightness = dot(color, threshold);

    //return the color if it was bright enough, otherwise return black
    if(brightness > 1.0f) return color;
    else return vec3(0.0f);
}