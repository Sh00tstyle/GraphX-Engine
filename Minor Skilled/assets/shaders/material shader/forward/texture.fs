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
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D emission;
    sampler2D reflection;
    sampler2D height;

    float shininess;
    float refractionFactor;
    float heightScale;

    bool hasSpecular;
    bool hasNormal;
    bool hasReflection;
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

uniform samplerCube environmentMap;
uniform sampler2D shadowMap;
uniform samplerCube shadowCubemaps[5];

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

vec3 GetSpecular(vec2 texCoord);
vec3 GetNormal(vec2 texCoord);
vec3 GetReflection(vec3 normal, vec2 texCoord, out bool hasReflection);
vec2 ParallaxMapping();

vec3 CalculateDirectionalLight(Light light, vec3 diff, vec3 spec, vec3 normal, vec3 viewDirection, vec2 texCoord, float shadow);
vec3 CalculatePointLight(Light light, vec3 diff, vec3 spec, vec3 normal, vec3 viewDirection, vec2 texCoord, float shadow);
vec3 CalculateSpotLight(Light light, vec3 diff, vec3 spec, vec3 normal, vec3 viewDirection, vec2 texCoord, float shadow);

float CalculateShadow(vec3 normal);
float CalculateCubemapShadow(vec3 normal, vec3 fragPos, int index);

vec4 CalculateBrightColor(vec3 color);

void main() {
    vec3 viewDirection = normalize(cameraPos - fs_in.fragPosWorld);

    //parallax mapping
    vec2 texCoord = ParallaxMapping();
    if(material.hasHeight && (texCoord.x > 1.0f || texCoord.y > 1.0f || texCoord.x < 0.0f || texCoord.y < 0.0f)) discard; //cutoff edges to avoid artifacts when using parallax mapping

    //get values from textures
    vec3 diffuse = texture(material.diffuse, texCoord).rgb;
    vec3 specular = GetSpecular(texCoord);
    vec3 normal = GetNormal(texCoord);

    //reflection
    bool hasReflection; //gets filled by the GetReflection function
    vec3 reflection = GetReflection(normal, texCoord, hasReflection);

    if(hasReflection) {
        //output reflection and ignore everything else
        fragColor = vec4(reflection, 1.0f);
        brightColor = vec4(vec3(0.0f), 1.0f);
        return;
    }

    //blend modes
    float alpha = 1.0f; //default (opaque)

    switch(material.blendMode) {
        case CUTOUT:
            //discard fragment with alpha below or equal to 0.1f
            alpha = texture(material.diffuse, texCoord).a;
            if(alpha <= 0.1f) discard;
            break;

        case TRANSPARENT:
            //sample alpha so it can be applied to the frag color
            alpha = texture(material.diffuse, texCoord).a;
            break;
    }

    //shadows
    float shadow = CalculateShadow(normal);

    for(int i = 0; i < usedCubeShadows; i++) {
        shadow += CalculateCubemapShadow(normal, fs_in.fragPosWorld, i);
    }

    if(shadow > 1.0f) shadow = 1.0f;
    shadow = 1.0f - shadow;

    //lighting
    vec3 result = vec3(0.0f);

    for(int i = 0; i < usedLights; i++) {
        switch(lights[i].type) {
            case DIRECTIONAL:
                result += CalculateDirectionalLight(lights[i], diffuse, specular, normal, viewDirection, texCoord, shadow);
                break;

            case POINT:
                result += CalculatePointLight(lights[i], diffuse, specular, normal, viewDirection, texCoord, shadow);
                break;

            case SPOT:
                result += CalculateSpotLight(lights[i], diffuse, specular, normal, viewDirection, texCoord, shadow);
                break;
        }
    }

    if(usedLights == 0) { //in case we have no light, simply sample from the diffuse map
        result = texture(material.diffuse, texCoord).rgb;
    }

    //emission
    vec3 emission = texture(material.emission, texCoord).rgb; //if there is no emission map, nothing will be added
    result += emission;

    fragColor = vec4(result, alpha);
    brightColor = CalculateBrightColor(result);
}

vec3 GetSpecular(vec2 texCoord) {
    //take the specular contribution from the specular map, otherwise use vec3(0.2f)
    vec3 specular;

    if(material.hasSpecular) {
        specular = texture(material.specular, texCoord).rgb;
    } else {
        specular = vec3(0.2f);
    }

    return specular;
}

vec3 GetNormal(vec2 texCoord) {
    //take the normal from the normal map if there is one, otherwise use frag normal
    vec3 normal;

    if(material.hasNormal) {
        normal = texture(material.normal, texCoord).rgb; //range [0, 1]
        normal = normalize(normal * 2.0f - 1.0f); //bring to range [-1, 1]
        normal = normalize(fs_in.TBN * normal); //transform normal from tangent to world space
    } else {
        normal = normalize(fs_in.fragNormal);
    }

    return normal;
}

vec3 GetReflection(vec3 normal, vec2 texCoord, out bool hasReflection) {
    if(!material.hasReflection) return vec3(0.0f);

    float reflectionAmount = texture(material.reflection, texCoord).r;

    if(reflectionAmount <= 0.01f) {
        hasReflection = false;
        return vec3(0.0f);
    } else {
        hasReflection = true;
    }

    vec3 I = normalize(fs_in.fragPosWorld - cameraPos);
    vec3 R;

    if(material.refractionFactor > 0.0f) {
        //use refraction if the factor is not 0
        float ratio = 1.0f / material.refractionFactor;

        R = refract(I, normalize(normal), ratio);
    } else {
        //use reflection
        R = reflect(I, normalize(normal));
    }

    //sample from dynamic environment map
    return texture(environmentMap, R).rgb;
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

vec3 CalculateDirectionalLight(Light light, vec3 diff, vec3 spec, vec3 normal, vec3 viewDirection, vec2 texCoord, float shadow) {
    //ambient
    vec3 ambient = light.ambient.rgb * diff;

    //diffuse
    float difference = max(dot(normal, -light.direction.xyz), 0.0f);
    vec3 diffuse = light.diffuse.rgb * difference * diff;

    //specular
    vec3 halfwayDireciton = normalize(light.direction.xyz + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular.rgb * specularity * spec;

    //combine results
    return (ambient + shadow * (diffuse + specular));
}

vec3 CalculatePointLight(Light light, vec3 diff, vec3 spec, vec3 normal, vec3 viewDirection, vec2 texCoord, float shadow) {
    vec3 lightDirection = normalize(light.position.xyz - fs_in.fragPosWorld);

    //ambient
    vec3 ambient = light.ambient.rgb * diff;

    //diffuse
    float difference = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = light.diffuse.rgb * difference * diff;

    //specular
    vec3 halfwayDireciton = normalize(lightDirection + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular.rgb * specularity * spec;

    //attenuation
    float distance = length(light.position.xyz - fs_in.fragPosWorld);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //combine results
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + shadow * (diffuse + specular));
}

vec3 CalculateSpotLight(Light light, vec3 diff, vec3 spec, vec3 normal, vec3 viewDirection, vec2 texCoord, float shadow) {
    vec3 lightDirection = normalize(light.position.xyz - fs_in.fragPosWorld);

    //ambient
    vec3 ambient = light.ambient.rgb * diff;

    //diffuse
    float difference = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = light.diffuse.rgb * difference * diff;

    //specular
    vec3 halfwayDireciton = normalize(lightDirection + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular.rgb * specularity * spec;

    //attenuation
    float distance = length(light.position.xyz - fs_in.fragPosWorld);
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

vec4 CalculateBrightColor(vec3 color) {
    const vec3 threshold = vec3(0.2126f, 0.7152f, 0.0722f);

    float brightness = dot(color, threshold);

    //return the color if it was bright enough, otherwise return black
    if(brightness > 1.0f) return vec4(color, 1.0f);
    else return vec4(vec3(0.0f), 1.0f);
}