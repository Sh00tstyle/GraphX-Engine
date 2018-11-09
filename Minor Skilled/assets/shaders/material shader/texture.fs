#version 330 core

const int LIGHTAMOUNT = 10;

const int DIRECTIONAL = 0;
const int POINT = 1;
const int SPOT = 2;

struct Light {
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
};

in VS_OUT {
    vec2 texCoord;
    
    vec3 tangentLightPos[LIGHTAMOUNT];
    vec3 tangentLightDir[LIGHTAMOUNT];

    vec3 tangentViewPos;
    vec3 tangentFragPos;
    vec3 tangentFragNormal; //needed in case there is no normal map
} fs_in;

uniform Light lights[LIGHTAMOUNT];
uniform Material material;

out vec4 fragColor;

vec3 GetSpecular();
vec3 GetNormal();
vec2 ParallaxMapping(vec3 viewDirection);

vec3 CalculateDirectionalLight(Light light, vec3 lightDir, vec3 normal, vec3 viewDirection, vec2 texCoord);
vec3 CalculatePointLight(Light light, vec3 lightPos, vec3 normal, vec3 viewDirection, vec2 texCoord);
vec3 CalculateSpotLight(Light light, vec3 lightPos, vec3 lightDir, vec3 normal, vec3 viewDirection, vec2 texCoord);

void main() {
    vec3 normal = GetNormal();
    vec3 viewDirection = normalize(fs_in.tangentViewPos - fs_in.tangentFragPos);

    //parallax mapping
    vec2 texCoord = ParallaxMapping(viewDirection);
    if(material.hasHeight && (texCoord.x > 1.0f || texCoord.y > 1.0f || texCoord.x < 0.0f || texCoord.y < 0.0f)) discard; //cutoff edges to avoid artifacts when using parallax mapping

    //lighting (calculated in tangent space)
    vec3 result = vec3(0.0f);

    for(int i = 0; i < LIGHTAMOUNT; i++) {
        if(lights[i].type == DIRECTIONAL) {
            result += CalculateDirectionalLight(lights[i], fs_in.tangentLightDir[i], normal, viewDirection, texCoord);
        } else if(lights[i].type == POINT) {
            result += CalculatePointLight(lights[i], fs_in.tangentLightPos[i], normal, viewDirection, texCoord);
        } else if(lights[i].type == SPOT) {
            result += CalculateSpotLight(lights[i], fs_in.tangentLightPos[i], fs_in.tangentLightDir[i], normal, viewDirection, texCoord);
        }
    }

    //emission
    vec3 emission = texture(material.emission, texCoord).rgb; //if there is no emission map, nothing will be added
    result += emission;

    fragColor = vec4(result, 1.0f);
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
    vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;

    //diffuse
    float difference = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * difference * texture(material.diffuse, texCoord).rgb;

    //specular
    vec3 halfwayDireciton = normalize(lightDir + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular * specularity * GetSpecular();

    //combine results
    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(Light light, vec3 lightPos, vec3 normal, vec3 viewDirection, vec2 texCoord) {
    vec3 lightDirection = normalize(lightPos - fs_in.tangentFragPos);

    //ambient
    vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;

    //diffuse
    float difference = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = light.diffuse * difference * texture(material.diffuse, texCoord).rgb;

    //specular
    vec3 halfwayDireciton = normalize(lightDirection + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular * specularity * GetSpecular();

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
    vec3 ambient = light.ambient * texture(material.diffuse, texCoord).rgb;

    //diffuse
    float difference = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = light.diffuse * difference * texture(material.diffuse, texCoord).rgb;

    //specular
    vec3 halfwayDireciton = normalize(lightDirection + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular * specularity * GetSpecular();

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