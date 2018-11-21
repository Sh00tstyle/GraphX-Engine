#version 460 core

in vec2 texCoord;

uniform float gamma;
uniform float exposure;

uniform int msaaSamples;
uniform int screenWidth;
uniform int screenHeight;

uniform sampler2DMS multiSampledScreenTexture;
uniform sampler2D bloomBlur;

out vec4 fragColor;

vec3 MSAA();
vec3 Bloom();
vec3 ExposureTonemap(vec3 color);
vec3 GammaCorrect(vec3 color);

void main() {
    vec3 color = MSAA();
    color += Bloom(); //additive blending
    color = ExposureTonemap(color);
    color = GammaCorrect(color);

    fragColor = vec4(color, 1.0f);
}

vec3 MSAA() {
    vec4 resultColor = vec4(0.0f);

    //sampling using texel fetch requires integer screen coordinates
    ivec2 viewportCoord = ivec2(screenWidth, screenHeight); 
    viewportCoord.x = int(viewportCoord.x * texCoord.x);
    viewportCoord.y = int(viewportCoord.y * texCoord.y);

    for(int i = 0; i < msaaSamples; i++) {
        resultColor += texelFetch(multiSampledScreenTexture, viewportCoord, i);
    }

    float samples = msaaSamples;
    resultColor /= samples; //average results for simeple MSAA effect

    return resultColor.rgb;
}

vec3 Bloom() {
    return texture(bloomBlur, texCoord).rgb;
}

vec3 ExposureTonemap(vec3 color) {
    return vec3(1.0f) - exp(-color * exposure);
}

vec3 GammaCorrect(vec3 color) {
    return pow(color, vec3(1.0f / gamma));
}