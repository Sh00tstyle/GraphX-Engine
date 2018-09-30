#version 330 core

struct Light {
    vec3 position;
    vec3 color;

    float linear;
    float quadratic;

    float radius; //light volume
};

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpecular;

const int LIGHT_AMOUNT = 32;
uniform Light lights[LIGHT_AMOUNT];
uniform vec3 cameraPos;

out vec4 fragColor;

void main() {
    //get data from the G-Buffer
    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 normal = texture(gNormal, texCoord).rgb;
    vec3 albedo = texture(gAlbedoSpecular, texCoord).rgb;
    float specIntensity = texture(gAlbedoSpecular, texCoord).a;

    //calculate lighting
    vec3 lighting = albedo * 0.1; //hard coded ambient color component
    vec3 lookDirection = normalize(cameraPos - fragPos);

    for(int i = 0; i < LIGHT_AMOUNT; i++) {
        float distance = length(lights[i].position - fragPos);
        if(distance > lights[i].radius) continue; //skip lighting calculation if the light is out of range

        //diffuse
        vec3 lightDirection = normalize(lights[i].position - fragPos);
        vec3 diffuse = max(dot(normal, lightDirection), 0.0f) * albedo * lights[i].color;

        //specular
        vec3 halfwayDir = normalize(lightDirection + lookDirection);  
        float specularity = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].color * specularity * specIntensity;

        //attenuation
        float attenuation = 1.0 / (1.0 + lights[i].linear * distance + lights[i].quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;

        lighting += diffuse + specular;
    }

    fragColor = vec4(lighting, 1.0f);
}