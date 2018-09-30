#version 330 core

struct Light {
    vec3 position;
    vec3 color;
    
    float linear;
    float quadratic;
};

in vec2 texCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

uniform Light light;

out vec4 fragColor;

void main() {             
    //retrieve data from gbuffer
    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 normal = texture(gNormal, texCoord).rgb;
    vec3 albedo = texture(gAlbedo, texCoord).rgb;
    float ambientOcclusion = texture(ssao, texCoord).r;
    
    //then calculate lighting as usual
    vec3 ambient = vec3(0.3f * albedo * ambientOcclusion);
    vec3 lighting  = ambient; 
    vec3 lookDirection  = normalize(-fragPos); //viewpos is (0.0.0)

    //diffuse
    vec3 lightDirection = normalize(light.position - fragPos);
    vec3 diffuse = max(dot(normal, lightDirection), 0.0f) * albedo * light.color;

    //specular
    vec3 halfwayDir = normalize(lightDirection + lookDirection);  
    float specularity = pow(max(dot(normal, halfwayDir), 0.0f), 8.0f);
    vec3 specular = light.color * specularity;

    //attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;

    fragColor = vec4(lighting, 1.0f);
}