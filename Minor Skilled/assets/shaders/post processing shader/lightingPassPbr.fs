#version 460 core

const float PI = 3.14159265359f;

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

uniform sampler2D gPositionMetallic;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoF0r;
uniform sampler2D gIrradianceF0g;
uniform sampler2D gPrefilterF0b;
uniform sampler2D gEmissionAO;
uniform sampler2D gDepth;

uniform sampler2D ssao;
uniform sampler2D brdfLUT;
uniform sampler2D shadowMap;
uniform samplerCube shadowCubemaps[5];

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

//PBR equations
vec3 FresnelSchlick(vec3 H, vec3 V, vec3 F0);
vec3 FresnelSchlickRoughness(vec3 N, vec3 V, vec3 F0, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

//shadow calculations
float CalculateShadow(vec3 normal, vec3 fragPos, vec4 lightSpaceFragPos);
float CalculateCubemapShadow(vec3 normal, vec3 fragPos, int index);

//helper functions
vec3 CalculateBrightColor(vec3 color);

//lighting
vec3 CalculateDirectionalLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic);
vec3 CalculatePointLight(Light light, vec3 fragPos, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic);
vec3 CalculateSpotLight(Light light, vec3 fragPos, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic);

void main() {
    //PBR lighting shader, using value names which are used in the equations for better understanding

    //sample data from the gBuffer textures
    vec3 fragPos = texture(gPositionMetallic, texCoord).rgb;
    vec3 normal = texture(gNormalRoughness, texCoord).rgb;
    vec3 albedo = texture(gAlbedoF0r, texCoord).rgb;
    float metallic = texture(gPositionMetallic, texCoord).a;
    float roughness = texture(gNormalRoughness, texCoord).a;
    float ao = texture(gEmissionAO, texCoord).a + texture(ssao, texCoord).r; //add material AO and SSAO

    brightColor.a = texture(gDepth, texCoord).r;

    if(ao > 1.0f) ao = 1.0f;

    //transform to world pos
    vec3 worldFragPos = vec3(inverse(viewMatrix) * vec4(fragPos, 1.0f));
    vec3 worldNormal = vec3(inverse(viewMatrix) * vec4(normal, 0.0f));

    //input lighting data
    vec3 N = worldNormal;
    vec3 V = normalize(cameraPos - worldFragPos); //view direction

    //shadows
    vec4 lightSpaceFragPos = lightSpaceMatrix * vec4(worldFragPos, 1.0f);

    float shadow = CalculateShadow(worldNormal, worldFragPos, lightSpaceFragPos);

    for(int i = 0; i < usedCubeShadows; i++) {
        shadow += CalculateCubemapShadow(N, worldFragPos, i);
    }

    if(shadow > 1.0f) shadow = 1.0f;
    shadow = 1.0f - shadow;

    //reflectance at normal incidence (directly looking at the surface)
    vec3 F0;
    F0.r = texture(gAlbedoF0r, texCoord).a;
    F0.g = texture(gIrradianceF0g, texCoord).a;
    F0.b = texture(gPrefilterF0b, texCoord).a;
    F0 = mix(F0, albedo, metallic); //use 0.04 for dielectrics (like plastic) and the albedo color for conductors (metals)

    //reflectance equation
    vec3 Lo = vec3(0.0f); //outgoing radiance

    for(int i = 0; i < usedLights; i++) {
        switch(lights[i].type) {
            case DIRECTIONAL:
                Lo += CalculateDirectionalLight(lights[i], V, N, F0, albedo, roughness, metallic);
                break;

            case POINT:
                Lo += CalculatePointLight(lights[i], worldFragPos, V, N, F0, albedo, roughness, metallic);
                break;

            case SPOT:
                Lo += CalculateSpotLight(lights[i], worldFragPos, V, N, F0, albedo, roughness, metallic);
                break;
        }
    }

    //ambient lighting
    vec3 F = FresnelSchlickRoughness(N, V, F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0f - kS;
    kD *= 1.0f - metallic;

    //sample from the irradiance map for the diffuse
    vec3 irradiance = texture(gIrradianceF0g, texCoord).rgb;
    vec3 diffuse = irradiance * albedo;

    //sample from the prefilter map and the BRDF lut and combine the results
    vec3 prefilteredColor = texture(gPrefilterF0b, texCoord).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0f), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    //apply IBL to the ambient color
    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo * shadow; //add results and apply shadow

    //emission
    vec3 emission = texture(gEmissionAO, texCoord).rgb; //if there is no emission map, nothing will be added
    color += emission;

    fragColor = vec4(color, 1.0f);
    brightColor.rgb = CalculateBrightColor(color);
}

//PBR equations
vec3 FresnelSchlick(vec3 H, vec3 V, vec3 F0) { //fresnel equation
    //ratio between specular and diffuse reflection (or in other words: reflection and refraction)
    float HdotV = max(dot(H, V), 0.0f); //angle between halfway vector and view direction

    return max(F0 + (1.0f - F0) * pow(1.0f - HdotV, 5.0f), 0.0f);
}

vec3 FresnelSchlickRoughness(vec3 N, vec3 V, vec3 F0, float roughness) { //fresnel equation (including roughness)
    //ratio between specular and diffuse reflection (or in other words: reflection and refraction)
    float NdotV = max(dot(N, V), 0.0f); //angle between halfway vector and view direction

    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - NdotV, 5.0f);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) { //normal distribution function
    //approximates how many microfacets are directily aligned to the halfway vector based on the roughness
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0f); //angle between normal and halfway vector
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) { //geometry sub function
    //approximates surface areas where mircofacets overshadow or obstruct each other based on the roughness
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;

    float num = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) { //geometry function
    float NdotV = max(dot(N, V), 0.0f); //angle between normal and view direction
    float NdotL = max(dot(N, L), 0.0f); //angle between normal and light direction

    float ggx2 = GeometrySchlickGGX(NdotV, roughness); //geometry obstruction
    float ggx1 = GeometrySchlickGGX(NdotL, roughness); //geometry overshadowing

    return ggx1 * ggx2;
}

//shadow calculations
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

//helper functions
vec3 CalculateBrightColor(vec3 color) {
    const vec3 threshold = vec3(0.2126f, 0.7152f, 0.0722f);

    float brightness = dot(color, threshold);

    //return the color if it was bright enough, otherwise return black
    if(brightness > 1.0f) return color;
    else return vec3(0.0f);
}

//lighting
vec3 CalculateDirectionalLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic) {
    vec3 L = normalize(-light.direction.xyz); //light direction
    vec3 H = normalize(V + L); //halfway vector

    float NdotL = max(dot(N, L), 0.0f); //angle between normal and light direction (used to scale the light)

    //per lights radiance
    vec3 radiance = light.diffuse.rgb; //lights radiance

    //Cook-Torrance BRDF (Bidirectional Reflectance Distribution Function)
    vec3 F = FresnelSchlick(H, V, F0); //Fresnel equation
    float NDF = DistributionGGX(N, H, roughness); //Normal distribution function
    float G = GeometrySmith(N, V, L, roughness); //Geometry function

    vec3 num = NDF * G * F; //numerator of the BRDF
    float denom = 4.0f * max(dot(N, V), 0.0f) * NdotL; //denominator of the BRDF
    vec3 specular = num / max(denom, 0.001f); //0.001 to avoid division by 0

    //reflection and refraction ratio respecting energy conservation
    vec3 kS = F; //specular reflection (reflection)
    vec3 kD = vec3(1.0f) - kS; //diffuse reflection (refraction)
    kD *= 1.0f - metallic; //nullify refraction if the surface is metallic, since metallics do not have refraction

    //add to total outgoing radiance Lo
    return (kD * albedo / PI + specular) * radiance * NdotL; //no need to multiply with kS, since it's already included in the BRDF
}

vec3 CalculatePointLight(Light light, vec3 fragPos, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic) {
    vec3 L = normalize(light.position.xyz - fragPos); //light direction
    vec3 H = normalize(V + L); //halfway vector

    float NdotL = max(dot(N, L), 0.0f); //angle between normal and light direction (used to scale the light)

    //per lights radiance
    float lightDistance = length(light.position.xyz - fragPos); //distance between light and surface/fragment
    float attenuation = 1.0f / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));
    vec3 radiance = light.diffuse.rgb * attenuation; //lights radiance

    //Cook-Torrance BRDF (Bidirectional Reflectance Distribution Function)
    vec3 F = FresnelSchlick(H, V, F0); //Fresnel equation
    float NDF = DistributionGGX(N, H, roughness); //Normal distribution function
    float G = GeometrySmith(N, V, L, roughness); //Geometry function

    vec3 num = NDF * G * F; //numerator of the BRDF
    float denom = 4.0f * max(dot(N, V), 0.0f) * NdotL; //denominator of the BRDF
    vec3 specular = num / max(denom, 0.001f); //0.001 to avoid division by 0

    //reflection and refraction ratio respecting energy conservation
    vec3 kS = F; //specular reflection (reflection)
    vec3 kD = vec3(1.0f) - kS; //diffuse reflection (refraction)
    kD *= 1.0f - metallic; //nullify refraction if the surface is metallic, since metallics do not have refraction

    //add to total outgoing radiance Lo
    return (kD * albedo / PI + specular) * radiance * NdotL; //no need to multiply with kS, since it's already included in the BRDF
}

vec3 CalculateSpotLight(Light light, vec3 fragPos, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic) {
    vec3 L = normalize(light.position.xyz - fragPos); //light direction
    vec3 H = normalize(V + L); //halfway vector

    float NdotL = max(dot(N, L), 0.0f); //angle between normal and light direction (used to scale the light)

    //per lights radiance
    float lightDistance = length(light.position.xyz - fragPos); //distance between light and surface/fragment
    float attenuation = 1.0f / (light.constant + light.linear * lightDistance + light.quadratic * (lightDistance * lightDistance));
    vec3 radiance = light.diffuse.rgb * attenuation; //lights radiance

    //Cook-Torrance BRDF (Bidirectional Reflectance Distribution Function)
    vec3 F = FresnelSchlick(H, V, F0); //Fresnel equation
    float NDF = DistributionGGX(N, H, roughness); //Normal distribution function
    float G = GeometrySmith(N, V, L, roughness); //Geometry function

    vec3 num = NDF * G * F; //numerator of the BRDF
    float denom = 4.0f * max(dot(N, V), 0.0f) * NdotL; //denominator of the BRDF
    vec3 specular = num / max(denom, 0.001f); //0.001 to avoid division by 0

    //reflection and refraction ratio respecting energy conservation
    vec3 kS = F; //specular reflection (reflection)
    vec3 kD = vec3(1.0f) - kS; //diffuse reflection (refraction)
    kD *= 1.0f - metallic; //nullify refraction if the surface is metallic, since metallics do not have refraction

    //spotlight
    float theta = dot(L, normalize(light.direction.xyz));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float spotlightIntensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

    //ambient
    vec3 ambient = light.ambient.rgb * albedo;

    //add to total outgoing radiance Lo
    return (kD * albedo / PI + specular) * radiance * NdotL * spotlightIntensity; //no need to multiply with kS, since it's already included in the BRDF
}