#version 330 core

const int LIGHTAMOUNT = 10; //TODO: replace with uniform buffer objects

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
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;

    vec4 lightSpaceFragPos;
} fs_in;

uniform vec3 cameraPos;

uniform Light lights[LIGHTAMOUNT];
uniform Material material;

uniform vec3 directionalLightPos;
uniform sampler2D shadowMap;

out vec4 fragColor;

vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDirection);
vec3 CalculatePointLight(Light light, vec3 normal, vec3 viewDirection);
vec3 CalculateSpotLight(Light light, vec3 normal, vec3 viewDirection);

float CalculateShadow(vec3 normal);

void main() {
    vec3 normal = normalize(fs_in.fragNormal);
    vec3 viewDirection = normalize(cameraPos - fs_in.fragPos);

    //lighting
    vec3 result = vec3(0.0f);

    for(int i = 0; i < LIGHTAMOUNT; i++) {
        switch(lights[i].type) {
            case DIRECTIONAL:
                result += CalculateDirectionalLight(lights[i], normal, viewDirection);
                break;

            case POINT:
                result += CalculatePointLight(lights[i], normal, viewDirection);
                break;

            case SPOT:
                result += CalculateSpotLight(lights[i], normal, viewDirection);
                break;
        }
    }

    //shadows
    float shadow = CalculateShadow(normal);
    shadow = 1.0f - shadow;
    result *= shadow;

    fragColor = vec4(result, 1.0f);
}

vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDirection) {
    //ambient
    vec3 ambient = light.ambient * material.ambient;

    //diffuse
    float difference = max(dot(normal, light.direction), 0.0f);
    vec3 diffuse = light.diffuse * difference * material.diffuse;

    //specular
    vec3 halfwayDireciton = normalize(light.direction + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular * specularity * material.specular;

    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - fs_in.fragPos);

    //ambient
    vec3 ambient = light.ambient * material.ambient;

    //diffuse
    float difference = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = light.diffuse * difference * material.diffuse;

    //specular
    vec3 halfwayDireciton = normalize(lightDirection + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular * specularity * material.specular;

    //attenuation
    float distance = length(light.position - fs_in.fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //combine results
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(Light light, vec3 normal, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - fs_in.fragPos);

    //ambient
    vec3 ambient = light.ambient * material.ambient;

    //diffuse
    float difference = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = light.diffuse * difference * material.diffuse;

    //specular
    vec3 halfwayDireciton = normalize(lightDirection + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular * specularity * material.specular;

    //attenuation
    float distance = length(light.position - fs_in.fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //spotlight
    float theta = dot(lightDirection, normalize(light.direction));
    float epsilon = light.innerCutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0f, 1.0f);

    //combine results
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}

float CalculateShadow(vec3 normal) {
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