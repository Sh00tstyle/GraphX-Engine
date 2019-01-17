#version 460 core

//array of offset directions for sampling
vec3 gridSamplingDisk[20] = vec3[] (
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

const float PI = 3.14159265359f;

//light types
const int DIRECTIONAL = 0;
const int POINT = 1;
const int SPOT = 2;

//blend modes
const int OPAQUE = 0;
const int CUTOUT = 1;
const int TRANSPARENT = 2;

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
    sampler2D emission;
    sampler2D height;

    vec3 F0;

    float refractionFactor;
    float heightScale;

    bool hasHeight;

    int blendMode;
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
    bool dirShadows;
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

//IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform float maxReflectionLod;

//shadows
uniform sampler2D shadowMap;
uniform samplerCube shadowCubemaps[5];

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec3 brightColor;

//PBR equations
vec3 FresnelSchlick(vec3 H, vec3 V, vec3 F0);
vec3 FresnelSchlickRoughness(vec3 N, vec3 V, vec3 F0, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

//shadow calculations
float CalculateShadow(vec3 normal);
float CalculateCubemapShadow(vec3 normal, vec3 fragPos, int index);

//helper functions
vec3 GetNormal(vec2 texCoord);
vec2 ParallaxMapping();
vec3 CalculateBrightColor(vec3 color);

//lighting
vec3 CalculateDirectionalLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic);
vec3 CalculatePointLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic);
vec3 CalculateSpotLight(Light light, vec3 V, vec3 N, vec3 F0, vec3 albedo, float roughness, float metallic);

void main() {
    //PBR lighting shader, using value names which are used in the equations for better understanding

    //parallax mapping
    vec2 texCoord = ParallaxMapping();
    //if(material.hasHeight && (texCoord.x > 1.0f || texCoord.y > 1.0f || texCoord.x < 0.0f || texCoord.y < 0.0f)) discard; //cutoff edges to avoid artifacts when using parallax mapping

    //get values
    vec3 albedo = texture(material.albedo, texCoord).rgb; //texture should be loaded in linear space
    float metallic = texture(material.metallic, texCoord).r;
    float roughness = texture(material.roughness, texCoord).r;
    float ao = texture(material.ao, texCoord).r;

    //input lighting data
    vec3 V = normalize(cameraPos - fs_in.fragPosWorld); //view direction
    vec3 N = GetNormal(texCoord); //normal
    vec3 R; //reflection/refraction

    if(material.refractionFactor > 0.0f) { //not sure if this is viable in PBR
        //refraction
        float ratio = 1.0f / material.refractionFactor;
        R = refract(-V, N, ratio);
    } else {
        //reflection
        R = reflect(-V, N);
    }

    //blend modes
    float alpha = 1.0f; //default (opaque)

    switch(material.blendMode) {
        case CUTOUT:
            //discard fragment with alpha below or equal to 0.1f
            alpha = texture(material.albedo, texCoord).a;
            if(alpha <= 0.1f) discard;
            break;

        case TRANSPARENT:
            //sample alpha so it can be applied to the frag color
            alpha = texture(material.albedo, texCoord).a;
            break;
    }

    //shadows
    float shadow = CalculateShadow(N);

    for(int i = 0; i < usedCubeShadows; i++) {
        shadow += CalculateCubemapShadow(N, fs_in.fragPosWorld, i);
    }

    if(shadow > 1.0f) shadow = 1.0f;
    shadow = 1.0f - shadow;

    //reflectance at normal incidence (directly looking at the surface)
    vec3 F0 = material.F0;
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

    //ambient lighting
    vec3 F = FresnelSchlickRoughness(N, V, F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0f - kS;
    kD *= 1.0f - metallic;

    //sample from the irradiance map for the diffuse
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    //sample from the prefilter map and the BRDF lut and combine the results
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * maxReflectionLod).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0f), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    //apply IBL to the ambient color
    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo * shadow; //add results and apply shadow

    //emission
    vec3 emission = albedo * texture(material.emission, texCoord).r; //if there is no emission map, nothing will be added
    color += emission;

    fragColor = vec4(color, alpha);
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
float CalculateShadow(vec3 normal) {
    if(!dirShadows) return 0.0f; //no shadows

    //perform perspective divide
    vec3 projectedCoords = fs_in.lightSpaceFragPos.xyz / fs_in.lightSpaceFragPos.w;

    //transform to [0,1] range
    projectedCoords = projectedCoords * 0.5f + 0.5f;

    //get closest depth value from lights perspective (using [0,1] range lightSpaceFragPos as coords)
    float closestDepth = texture(shadowMap, projectedCoords.xy).r; 

    //get depth of current fragment from lights perspective
    float currentDepth = projectedCoords.z;

    //calculate bias based on depth map resolution and slope
    vec3 lightDirection = normalize(directionalLightPos - fs_in.fragPosWorld);
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
vec3 GetNormal(vec2 texCoord) {
    //take the normal from the normal map and transform it to world space
    vec3 normal = texture(material.normal, texCoord).rgb; //range [0, 1]
    normal = normalize(normal * 2.0f - 1.0f); //bring to range [-1, 1]
    normal = normalize(fs_in.TBN * normal); //transform normal from tangent to world space

    return normal;
}

vec2 ParallaxMapping() {
    //use normal texCoords if there is no height map, otherwise apply parallax occulsion mapping
    if(!material.hasHeight) return fs_in.texCoord; 

    vec3 viewDirection = normalize(-fs_in.fragPosView);

    //number of depth layers
    const float minLayers = 8.0f;
    const float maxLayers = 32.0f;
    float layerAmount = mix(maxLayers, minLayers, abs(dot(vec3(0.0f, 0.0f, 1.0f), viewDirection)));

    //calculate the size of each layer
    float layerDepth = 1.0f / layerAmount;
    //depth of current layer
    float currentLayerDepth = 0.0f;
    //the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDirection.xy / viewDirection.z * material.heightScale; 
    vec2 deltaTexCoords = P / layerAmount;
  
    //get initial values
    vec2 currentTexCoords = fs_in.texCoord;
    float currentDepthMapValue = texture(material.height, currentTexCoords).r;
    
    while(currentLayerDepth < currentDepthMapValue) { //basically raycasting
        //shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        //get depthmap value at current texture coordinates
        currentDepthMapValue = texture(material.height, currentTexCoords).r;  
        //get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    //get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    //get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(material.height, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    //interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0f - weight);

    return finalTexCoords;
}

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