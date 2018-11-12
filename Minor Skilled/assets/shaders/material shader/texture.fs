#version 330 core

const int LIGHTAMOUNT = 10;

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
    sampler2D height;

    float shininess;
    float heightScale;

    bool hasSpecular;
    bool hasNormal;
    bool hasHeight;

    int blendMode;
};

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
    
    vec3 tangentLightPos[LIGHTAMOUNT];
    vec3 tangentLightDir[LIGHTAMOUNT];

    vec3 tangentViewPos;
    vec3 tangentFragPos;
    vec3 tangentFragNormal; //needed in case there is no normal map

    vec4 lightSpaceFragPos;
} fs_in;

layout (std140) uniform positionsBlock {
    vec3 cameraPos;
    vec3 directionalLightPos;
};

layout(std140) uniform lightsBlock {
    Light lights[LIGHTAMOUNT];
};

uniform Material material;
uniform sampler2D shadowMap;

out vec4 fragColor;

vec3 GetSpecular();
vec3 GetNormal();
vec2 ParallaxMapping(vec3 viewDirection);

vec3 CalculateDirectionalLight(Light light, vec3 lightDir, vec3 normal, vec3 viewDirection, vec2 texCoord);
vec3 CalculatePointLight(Light light, vec3 lightPos, vec3 normal, vec3 viewDirection, vec2 texCoord);
vec3 CalculateSpotLight(Light light, vec3 lightPos, vec3 lightDir, vec3 normal, vec3 viewDirection, vec2 texCoord);

float CalculateShadow();

void main() {
    vec3 normal = GetNormal();
    vec3 viewDirection = normalize(fs_in.tangentViewPos - fs_in.tangentFragPos);

    //parallax mapping
    vec2 texCoord = ParallaxMapping(viewDirection);
    if(material.hasHeight && (texCoord.x > 1.0f || texCoord.y > 1.0f || texCoord.x < 0.0f || texCoord.y < 0.0f)) discard; //cutoff edges to avoid artifacts when using parallax mapping

    //blend modes
    float alpha = 1.0f; //default (opaque)

    switch(material.blendMode) {
        case CUTOUT:
            //discard fragment with alpha below or equal to 0.1f
            alpha = texture(material.diffuse, fs_in.texCoord).a;
            if(alpha <= 0.1f) discard;
            break;

        case TRANSPARENT:
            //sample alpha so it can be applied to the frag color
            alpha = texture(material.diffuse, fs_in.texCoord).a;
            break;
    }

    //lighting (calculated in tangent space)
    vec3 result = vec3(0.0f);

    for(int i = 0; i < LIGHTAMOUNT; i++) {
        switch(lights[i].type) {
            case DIRECTIONAL:
                result += CalculateDirectionalLight(lights[i], fs_in.tangentLightDir[i], normal, viewDirection, texCoord);
                break;

            case POINT:
                result += CalculatePointLight(lights[i], fs_in.tangentLightPos[i], normal, viewDirection, texCoord);
                break;

            case SPOT:
                result += CalculateSpotLight(lights[i], fs_in.tangentLightPos[i], fs_in.tangentLightDir[i], normal, viewDirection, texCoord);
                break;
        }
    }

    //emission
    vec3 emission = texture(material.emission, texCoord).rgb; //if there is no emission map, nothing will be added
    result += emission;

    //shadows
    float shadow = CalculateShadow();
    shadow = 1.0f - shadow;
    result *= shadow * 0.5f;

    fragColor = vec4(result, alpha);
}

vec3 GetSpecular() {
    //take the specular contribution from the specular map, otherwise use vec3(0.2f)
    vec3 specular;

    if(material.hasSpecular) {
        specular = texture(material.specular, fs_in.texCoord).rgb;
    } else {
        specular = vec3(0.2f);
    }

    return specular;
}

vec3 GetNormal() {
    //take the normal from the normal map if there is one, otherwise use frag normal
    vec3 normal;

    if(material.hasNormal) {
        normal = texture(material.normal, fs_in.texCoord).rgb; //range [0, 1]
        normal = normalize(normal * 2.0f - 1.0f); //bring to range [-1, 1]
    } else {
        normal = normalize(fs_in.tangentFragNormal);
    }

    return normal;
}

vec2 ParallaxMapping(vec3 viewDirection) {
    //use normal texCoords if there is no height map, otherwise apply parallax occulsion mapping
    if(!material.hasHeight) return fs_in.texCoord; 

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

vec3 CalculateDirectionalLight(Light light, vec3 lightDir, vec3 normal, vec3 viewDirection, vec2 texCoord) {
    //ambient
    vec3 ambient = light.ambient.rgb * texture(material.diffuse, texCoord).rgb;

    //diffuse
    float difference = max(dot(normal, -lightDir), 0.0f);
    vec3 diffuse = light.diffuse.rgb * difference * texture(material.diffuse, texCoord).rgb;

    //specular
    vec3 halfwayDireciton = normalize(lightDir + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular.rgb * specularity * GetSpecular();

    //combine results
    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(Light light, vec3 lightPos, vec3 normal, vec3 viewDirection, vec2 texCoord) {
    vec3 lightDirection = normalize(lightPos - fs_in.tangentFragPos);

    //ambient
    vec3 ambient = light.ambient.rgb * texture(material.diffuse, texCoord).rgb;

    //diffuse
    float difference = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = light.diffuse.rgb * difference * texture(material.diffuse, texCoord).rgb;

    //specular
    vec3 halfwayDireciton = normalize(lightDirection + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular.rgb * specularity * GetSpecular();

    //attenuation
    float distance = length(lightPos - fs_in.tangentFragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //combine results
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(Light light, vec3 lightPos, vec3 lightDir, vec3 normal, vec3 viewDirection, vec2 texCoord) {
    vec3 lightDirection = normalize(lightPos - fs_in.tangentFragPos);

    //ambient
    vec3 ambient = light.ambient.rgb * texture(material.diffuse, texCoord).rgb;

    //diffuse
    float difference = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = light.diffuse.rgb * difference * texture(material.diffuse, texCoord).rgb;

    //specular
    vec3 halfwayDireciton = normalize(lightDirection + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular.rgb * specularity * GetSpecular();

    //attenuation
    float distance = length(lightPos - fs_in.tangentFragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //spotlight
    float theta = dot(lightDirection, normalize(lightDir));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

    //combine results
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

float CalculateShadow() {
    //perform perspective divide
    vec3 projectedCoords = fs_in.lightSpaceFragPos.xyz / fs_in.lightSpaceFragPos.w;

    //transform to [0,1] range
    projectedCoords = projectedCoords * 0.5f + 0.5f;

    //get closest depth value from lights perspective (using [0,1] range lightSpaceFragPos as coords)
    float closestDepth = texture(shadowMap, projectedCoords.xy).r; 

    //get depth of current fragment from lights perspective
    float currentDepth = projectedCoords.z;

    //calculate bias based on depth map resolution and slope
    vec3 normal = normalize(fs_in.fragNormal);
    vec3 lightDirection = normalize(directionalLightPos - fs_in.fragPos);
    float bias = max(0.05f * (1.0f - dot(normal, lightDirection)), 0.005f);

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