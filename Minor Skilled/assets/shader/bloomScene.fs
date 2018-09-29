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

uniform Light lights[4];
uniform sampler2D diffuseTexture;
uniform vec3 cameraPos;

//multiple rendertargets (color attachments)
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main() {           
    vec3 color = texture(diffuseTexture, fs_in.texCoord).rgb;
    vec3 normal = normalize(fs_in.fragNormal);

    //ambient
    vec3 ambient = 0.0 * color;

    //lighting
    vec3 lighting = vec3(0.0);
    vec3 lookDirection = normalize(cameraPos - fs_in.fragPos);

    for(int i = 0; i < 4; i++) {
        //diffuse
        vec3 lightDirection = normalize(lights[i].position - fs_in.fragPos);
        float difference = max(dot(lightDirection, normal), 0.0);
        vec3 result = lights[i].color * difference * color;  

        //attenuation (use quadratic as we have gamma correction)
        float distance = length(fs_in.fragPos - lights[i].position);
        result *= 1.0 / (distance * distance);
        lighting += result;       
    }

    vec3 result = ambient + lighting;

    //check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    
    if(brightness > 1.0) brightColor = vec4(result, 1.0);
    else brightColor = vec4(0.0, 0.0, 0.0, 1.0);

    fragColor = vec4(result, 1.0);
}