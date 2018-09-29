#version 330 core

in vec2 texCoord;

uniform sampler2D scene;
uniform sampler2D bloomBlur;

uniform bool useBloom;
uniform float exposure;

out vec4 fragColor;

void main() {             
    float gamma = 2.2;
    vec3 hdrColor = texture(scene, texCoord).rgb;      
    vec3 bloomColor = texture(bloomBlur, texCoord).rgb;

    if(useBloom) hdrColor += bloomColor; //additive blending

    //tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    //gamma correction       
    result = pow(result, vec3(1.0 / gamma));
    fragColor = vec4(result, 1.0);
}