#version 330 core

//material struct
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    float innerCutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

 //lighting
in vec3 fragPos;
in vec3 fragNormal;
in vec2 texCoords;

uniform vec3 cameraPos;
uniform Material material;

uniform DirLight dirLight;
#define POINT_LIGHT_AMT 4
uniform PointLight pointLights[POINT_LIGHT_AMT];
uniform SpotLight spotLight;

out vec4 fragColor;

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 lookDirection);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 lookDirection);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 lookDirection);

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lookDirection = normalize(cameraPos - fragPos);

    //add directional light
    vec3 result = CalculateDirLight(dirLight, normal, lookDirection);

    //add point lights
    for(int i = 0; i < POINT_LIGHT_AMT; i++) {
        result += CalculatePointLight(pointLights[i], normal, lookDirection);
    }

    //add spot light
    result += CalculateSpotLight(spotLight, normal, lookDirection);

    fragColor = vec4(result, 1.0f);
}

vec3 CalculateDirLight(DirLight light, vec3 normal, vec3 lookDirection) {
    vec3 lightDirection = normalize(-light.direction);

    //calculate ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));

    //calculate diffuse
    float difference = max(dot(normal, lightDirection), 0.0f); //cosine of the angle between the normal and the light direction (dot)
    vec3 diffuse = light.diffuse * difference * vec3(texture(material.diffuse, texCoords));

    //calculate specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularity = pow(max(dot(lookDirection, reflectDirection), 0.0f), material.shininess);
    vec3 specular = light.specular * specularity * vec3(texture(material.specular, texCoords));

    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 lookDirection) {
    vec3 lightDirection = normalize(light.position - fragPos);

    //calculate ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));

    //calculate diffuse
    float difference = max(dot(normal, lightDirection), 0.0f); //cosine of the angle between the normal and the light direction (dot)
    vec3 diffuse = light.diffuse * difference * vec3(texture(material.diffuse, texCoords));

    //calculate specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularity = pow(max(dot(lookDirection, reflectDirection), 0.0f), material.shininess);
    vec3 specular = light.specular * specularity * vec3(texture(material.specular, texCoords));

    //calculate attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 lookDirection) {
    vec3 lightDirection = normalize(light.position - fragPos);

    //calculate ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));

    //calculate diffuse
    float difference = max(dot(normal, lightDirection), 0.0f); //cosine of the angle between the normal and the light direction (dot)
    vec3 diffuse = light.diffuse * difference * vec3(texture(material.diffuse, texCoords));

    //calculate specular
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularity = pow(max(dot(lookDirection, reflectDirection), 0.0f), material.shininess);
    vec3 specular = light.specular * specularity * vec3(texture(material.specular, texCoords));

    //calculate spotlight
    float theta = dot(lightDirection, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
    //float intensity = smoothstep(light.outerCutOff, light.innerCutOff, theta); //alternative

    ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;

    //calculate attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

   return (ambient + diffuse + specular);
}