#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
} fs_in;

uniform sampler2D floorTexture;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 cameraPos;
uniform bool useGamma;

out vec4 fragColor;

vec3 blinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor) {
    //ambient
    //vec3 ambient = 0.05f * color; //ambient not needed, since the texture is sampled in main and mutiplied with the result of this function

    //diffuse
    vec3 lightDirection = normalize(lightPos - fs_in.fragPos);
    float difference = max(dot(lightDirection, normal), 0.0f);
    vec3 diffuse = difference * lightColor;

    //specular
    vec3 lookDirection = normalize(cameraPos - fs_in.fragPos);
    vec3 halfwayDir = normalize(lightDirection + lookDirection);
    float specularity = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = specularity * lightColor;

    //simple attenuation
    float maxDistance = 1.5f;
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (useGamma ? distance * distance : distance);

    diffuse *= attenuation;
    specular *= attenuation;

    return (diffuse + specular);
}

void main() {
    vec3 color = texture(floorTexture, fs_in.texCoord).rgb;
    vec3 lighting = vec3(0.0f);

    for(int i = 0; i < 4; ++i) {
        lighting += blinnPhong(normalize(fs_in.fragNormal), fs_in.fragPos, lightPositions[i], lightColors[i]);
    }

    color *= lighting;

    if(useGamma) color = pow(color, vec3(1.0f/2.2f)); //apply gamma correction
    
    fragColor = vec4(color, 1.0f);
}