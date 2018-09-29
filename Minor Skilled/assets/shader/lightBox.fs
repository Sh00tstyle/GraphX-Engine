#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
} fs_in;

uniform vec3 lightColor;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main() {           
    fragColor = vec4(lightColor, 1.0);

    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0) brightColor = vec4(fragColor.rgb, 1.0);
}