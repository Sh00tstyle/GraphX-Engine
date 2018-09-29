#version 330 core

struct Light {
    vec3 position;
    vec3 color;
};

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
} fs_in;

uniform Light lights[16];
uniform sampler2D diffuseTexture;
uniform vec3 cameraPos;

out vec4 fragColor;

void main() {           
    vec3 color = texture(diffuseTexture, fs_in.texCoord).rgb;
    vec3 normal = normalize(fs_in.fragNormal);

    //ambient
    vec3 ambient = 0.0 * color;

    //lighting
    vec3 lighting = vec3(0.0);

    for(int i = 0; i < 16; i++)
    {
        //diffuse
        vec3 lightDirection = normalize(lights[i].position - fs_in.fragPos);
        float difference = max(dot(lightDirection, normal), 0.0);
        vec3 diffuse = lights[i].color * difference * color;      
        vec3 result = diffuse;

        //attenuation (use quadratic as we have gamma correction)
        float distance = length(fs_in.fragPos - lights[i].position);
        result *= 1.0 / (distance * distance);
        lighting += result;
                
    }

    fragColor = vec4(ambient + lighting, 1.0);
}