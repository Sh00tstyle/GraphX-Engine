#version 460 core

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
} fs_in;

uniform vec3 renderPos;

//diffuse
uniform bool useTexture;
uniform vec3 diffuseVec;
uniform sampler2D diffuseMap;

//lighting
uniform bool useLight;
uniform vec3 lightDirection;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;

out vec4 fragColor;

void main() {
    vec3 normal = normalize(fs_in.fragNormal);

    //color
    vec3 color;

    if(useTexture) color = texture(diffuseMap, fs_in.texCoord).rgb;
    else color = diffuseVec;

    //simple directional lighting
    if(useLight) {
        //ambient
        vec3 ambient = lightAmbient * color;

        //diffuse
        float difference = max(dot(normal, -lightDirection), 0.0f);
        vec3 diffuse = lightDiffuse * difference * color;

        //combine results
        color = ambient + diffuse;
    }

    fragColor = vec4(color, 1.0f);
}