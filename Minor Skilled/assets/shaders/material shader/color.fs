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
} fs_in;

uniform vec3 cameraPos;
uniform Light lights[LIGHTAMOUNT];
uniform Material material;

out vec4 fragColor;

vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDirection);
vec3 CalculatePointLight(Light light, vec3 normal, vec3 viewDirection);
vec3 CalculateSpotLight(Light light, vec3 normal, vec3 viewDirection);

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