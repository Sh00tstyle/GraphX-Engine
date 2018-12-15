#version 460 core

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
};

in VS_OUT {
    vec3 fragPosWorld;
    vec3 fragPosView;
    vec3 fragNormalWorld;
    vec2 texCoord;

    mat3 TBN;
} fs_in;

layout (std140) uniform dataBlock {
    bool useShadows;
    int usedCubeShadows;
    float farPlane;

    vec3 cameraPos;
    vec3 directionalLightPos;

    vec3 pointLightPositions[5];
};

uniform Material material;
uniform float maxReflectionLod;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;

layout (location = 0) out vec4 gPositionMetallic;
layout (location = 1) out vec4 gNormalRoughness;
layout (location = 2) out vec4 gAlbedoF0r;
layout (location = 3) out vec4 gIrradianceF0g;
layout (location = 4) out vec4 gPrefilterF0b;
layout (location = 5) out vec4 gEmissionAO;
layout (location = 6) out vec3 gDepth;

vec3 GetNormal(vec2 texCoord);
vec2 ParallaxMapping();

void main() {
    vec3 viewDirection = normalize(cameraPos - fs_in.fragPosWorld); //camera pos is vec3(0.0f), since we are in view space

    //parallax mapping
    vec2 texCoord = ParallaxMapping();
    if(material.hasHeight && (texCoord.x > 1.0f || texCoord.y > 1.0f || texCoord.x < 0.0f || texCoord.y < 0.0f)) discard; //cutoff edges to avoid artifacts when using parallax mapping

    vec3 normal = GetNormal(texCoord);
    float roughness = texture(material.roughness, texCoord).r;

    vec3 R;

    if(material.refractionFactor > 0.0f) { //not sure if this is viable in PBR
        //refraction
        float ratio = 1.0f / material.refractionFactor;
        R = refract(-viewDirection, fs_in.fragNormalWorld, ratio);
    } else {
        //reflection
        R = reflect(-viewDirection, fs_in.fragNormalWorld);
    }

    //store the data in the gBuffer
    gPositionMetallic.rgb = fs_in.fragPosView;
    gPositionMetallic.a = texture(material.metallic, texCoord).r;

    gNormalRoughness.rgb = normal;
    gNormalRoughness.a = roughness;

    gAlbedoF0r.rgb = texture(material.albedo, texCoord).rgb;
    gAlbedoF0r.a = material.F0.r;

    gIrradianceF0g.rgb = texture(irradianceMap, normal).rgb;
    gIrradianceF0g.a = material.F0.g;

    gPrefilterF0b.rgb = textureLod(prefilterMap, R, roughness * maxReflectionLod).rgb;
    gPrefilterF0b.a = material.F0.b;

    gEmissionAO.rgb = texture(material.emission, texCoord).rgb;
    gEmissionAO.a = texture(material.ao, texCoord).r;

    gDepth.r = gl_FragCoord.z;
}

vec3 GetNormal(vec2 texCoord) {
    vec3 normal = texture(material.normal, texCoord).rgb; //range [0, 1]
    normal = normalize(normal * 2.0f - 1.0f); //bring to range [-1, 1]
    normal = normalize(fs_in.TBN * normal); //transform normal from tangent to view space 

    return normal;
}

vec2 ParallaxMapping() {
    //use normal texCoords if there is no height map, otherwise apply parallax occulsion mapping
    if(!material.hasHeight) return fs_in.texCoord; 

    vec3 viewDirection = normalize(fs_in.fragPosView);

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