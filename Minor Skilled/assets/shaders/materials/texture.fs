#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D emissionMap;

out vec4 fragColor;

void main() {
    //TODO: lighting calculation

    fragColor = vec4(texture(diffuseMap, fs_in.texCoord).rgb, 1.0f);
}