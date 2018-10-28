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

    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float innerCutoff;
    float outerCutoff;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D emission;
    float shininess;
};

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
} fs_in;

uniform vec3 cameraPos;
uniform Light lights[10];
uniform Material material;

out vec4 fragColor;

vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDirection);

void main() {
    vec3 normal = normalize(fs_in.fragNormal); //TODO: replace with normal map
    vec3 viewDirection = normalize(cameraPos - fs_in.fragPos);

    vec3 result = vec3(0.0f);

    for(int i = 0; i < LIGHTAMOUNT; i++) {
        if(lights[i].type == DIRECTIONAL) {
            result += CalculateDirectionalLight(lights[i], normal, viewDirection);
        } else if(lights[i].type == POINT) {
            //TODO
        } else if(lights[i].type == SPOT) {
            //TODO
        }
    }

    fragColor = vec4(result, 1.0f);
}

vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDirection) {
    //ambient
    vec3 ambient = light.ambient * texture(material.diffuse, fs_in.texCoord).rgb;

    //diffuse
    float difference = max(dot(normal, light.direction), 0.0f);
    vec3 diffuse = light.diffuse * (difference * texture(material.diffuse, fs_in.texCoord).rgb);

    //specular
    vec3 halfwayDireciton = normalize(light.direction + viewDirection); //blinn-phong
    float specularity = pow(max(dot(normal, halfwayDireciton), 0.0f), material.shininess);
    vec3 specular = light.specular * (specularity * texture(material.specular, fs_in.texCoord).rgb);

    return (ambient + diffuse + specular);
}