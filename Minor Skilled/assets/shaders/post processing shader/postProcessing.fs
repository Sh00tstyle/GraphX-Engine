#version 460 core

in vec2 texCoord;

uniform bool useFXAA;
uniform bool useBloom;

uniform float gamma;
uniform float exposure;

uniform vec2 inverseScreenTextureSize;
uniform float fxaaSpanMax;
uniform float fxaaReduceMin;
uniform float fxaaReduceMul;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;

out vec4 fragColor;

vec3 FXAA(vec3 color);
vec3 Bloom();
vec3 ExposureTonemap(vec3 color);
vec3 GammaCorrect(vec3 color);

void main() {
    vec3 color = texture(screenTexture, texCoord).rgb;

    //FXAA
    if(useFXAA) color = FXAA(color);

    //bloom
    if(useBloom) color += Bloom(); //additive blending

    //hdr tonemap
    color = ExposureTonemap(color);

    //gamma correct
    color = GammaCorrect(color);

    fragColor = vec4(color, 1.0f);
}

vec3 FXAA(vec3 color) {
    //information from: https://www.youtube.com/watch?v=Z9bYzpwVINA

    //vector to determine luminosity of a pixel
    vec3 luma = vec3(0.299f, 0.587f, 0.114f);

    //sample in a cross pattern and calculate the luminosity of each sampled pixel
    float lumaM = dot(luma, texture(screenTexture, texCoord).rgb); //middle
    float lumaTL = dot(luma, texture(screenTexture, texCoord + vec2(-1.0f, -1.0f) * inverseScreenTextureSize).rgb); //top left
    float lumaTR = dot(luma, texture(screenTexture, texCoord + vec2(1.0f, -1.0f) * inverseScreenTextureSize).rgb); //top right
    float lumaBL = dot(luma, texture(screenTexture, texCoord + vec2(-1.0f, 1.0f) * inverseScreenTextureSize).rgb); //bottom left
    float lumaBR = dot(luma, texture(screenTexture, texCoord + vec2(1.0f, 1.0f) * inverseScreenTextureSize).rgb); //bottom right

    //determine blur direction and detect edges (if the vectors x and y values are both 0, there is no edge)
    vec2 blurDirection = vec2(-((lumaTL + lumaTR) - (lumaBL + lumaBR)), (lumaTL + lumaBL) - (lumaTR + lumaBR));

    float directionReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * 0.25f * fxaaReduceMul, fxaaReduceMin); //prevent divsion by 0
    float inverseDirectionAdjustment = 1.0f / min(abs(blurDirection.x), abs(blurDirection.y)) + directionReduce; //determine smallest component

    //clamp blur direction to maximum blur range and convert to texel space
    blurDirection = min(vec2(fxaaSpanMax, fxaaSpanMax), 
                    max(vec2(-fxaaSpanMax, -fxaaSpanMax), blurDirection * inverseDirectionAdjustment)) * inverseScreenTextureSize; 

    //blur the edge pixels by sampling from the screen texture offset by the blur direction
    vec3 result1 =  (1.0f / 2.0f) * ( //smaller range
                    texture(screenTexture, texCoord + (blurDirection * vec2(1.0f / 3.0f - 0.5f))).rgb +
                    texture(screenTexture, texCoord + (blurDirection * vec2(2.0f / 3.0f - 0.5f))).rgb);

    vec3 result2 =  result1 * (1.0f / 2.0f) + (1.0f / 4.0f) * ( //bigger range
                    texture(screenTexture, texCoord + (blurDirection * vec2(0.0f / 3.0f - 0.5f))).rgb +
                    texture(screenTexture, texCoord + (blurDirection * vec2(3.0f / 3.0f - 0.5f))).rgb);

    //check if we sampled too far
    float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
    float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
    float lumaResult2 = dot(luma, result2);

    //return result1 if we sampled to far, otherwise return result2
    if(lumaResult2 < lumaMin || lumaResult2 > lumaMax) return result1;
    else return result2;
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