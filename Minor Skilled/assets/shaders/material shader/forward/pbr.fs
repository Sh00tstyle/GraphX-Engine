#version 460 core

const float PI = 3.14159265359;

//light types
const int DIRECTIONAL = 0;
const int POINT = 1;
const int SPOT = 2;

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

struct Material {
    sampler2D albedo;
    sampler2D normal;
    sampler2D metallic;
    sampler2D roughness;
    sampler2D ao;
};

in VS_OUT {
    vec3 fragPosWorld;
    vec3 fragPosView;
    vec3 fragNormal;
    vec2 texCoord;

    mat3 TBN;

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

uniform Material material;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

//PBR equations
vec3 FresnelSchlick(vec3 H, vec3 V, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

//helper functions
vec3 GetNormal(vec2 texCoord);

//lighting
vec3 CalculateDirectionalLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic);
vec3 CalculatePointLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic);
vec3 CalculateSpotLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic);

void main() {
    //PBR lighting shader, using value names which are used in the equations for better understanding

    //get values
    vec3 albedo = texture(material.albedo, fs_in.texCoord).rgb; //texture should be loaded in linear space
    float metallic = texture(material.metallic, fs_in.texCoord).r;
    float roughness = texture(material.roughness, fs_in.texCoord).r;
    float ao = texture(material.ao, fs_in.texCoord).r;

    vec3 V = normalize(cameraPos - fs_in.fragPosWorld); //view direction
    vec3 N = GetNormal(fs_in.texCoord); //normal

    //reflectance at normal incidence (directly looking at the surface)
    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic); //use 0.04 for dielectrics (like plastic) and the albedo color for conductors (metals)

    //reflectance equation
    vec3 Lo = vec3(0.0f); //outgoing radiance

    for(int i = 0; i < usedLights; i++) {
        switch(lights[i].type) {
            case DIRECTIONAL:
                Lo += CalculateDirectionalLight(lights[i], V, N, F0, albedo, roughness, metallic);
                break;

            case POINT:
                Lo += CalculatePointLight(lights[i], V, N, F0, albedo, roughness, metallic);
                break;

            case SPOT:
                Lo += CalculateSpotLight(lights[i], V, N, F0, albedo, roughness, metallic);
                break;
        }
    }

    //ambient lighting, is going to be replaced with environment lighting from IBL
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    fragColor = vec4(color, 1.0f);
    brightColor = vec4(vec3(0.0f), 1.0f);
}

//PBR equations
vec3 FresnelSchlick(vec3 H, vec3 V, vec3 F0) { //fresnel equation
    //ratio between specular and diffuse reflection (or in other words: reflection and refraction)
    float HdotV = max(dot(H, V), 0.0f); //angle between halfway vector and view direction

    return max(F0 + (1.0f - F0) * pow(1.0f - HdotV, 5.0f), 0.0f);
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

//helper functions
vec3 GetNormal(vec2 texCoord) {
    //take the normal from the normal map and transform it to world space
    vec3 normal = texture(material.normal, texCoord).rgb; //range [0, 1]
    normal = normalize(normal * 2.0f - 1.0f); //bring to range [-1, 1]
    normal = normalize(fs_in.TBN * normal); //transform normal from tangent to world space

    return normal;
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

vec3 CalculatePointLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic) {
    vec3 L = normalize(light.position.xyz - fs_in.fragPosWorld); //light direction
    vec3 H = normalize(V + L); //halfway vector

    float NdotL = max(dot(N, L), 0.0f); //angle between normal and light direction (used to scale the light)

    //per lights radiance
    float lightDistance = length(light.position.xyz - fs_in.fragPosWorld); //distance between light and surface/fragment
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

vec3 CalculateSpotLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic) {
    vec3 L = normalize(light.position.xyz - fs_in.fragPosWorld); //light direction
    vec3 H = normalize(V + L); //halfway vector

    float NdotL = max(dot(N, L), 0.0f); //angle between normal and light direction (used to scale the light)

    //per lights radiance
    float lightDistance = length(light.position.xyz - fs_in.fragPosWorld); //distance between light and surface/fragment
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