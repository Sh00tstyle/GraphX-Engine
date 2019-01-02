#version 460 core

in vec2 texCoord;

layout (std140) uniform matricesBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 previousViewProjectionMatrix;
    mat4 lightSpaceMatrix;
};

uniform bool useFXAA;
uniform bool useMotionBlur;
uniform bool useBloom;
uniform bool useSSR;

uniform bool ssrDebug;

uniform float gamma;
uniform float exposure;

uniform vec2 inverseScreenTextureSize;
uniform float fxaaSpanMax;
uniform float fxaaReduceMin;
uniform float fxaaReduceMul;

uniform sampler2D screenTexture;
uniform sampler2D sceneDepth;
uniform sampler2D bloomBlur;
uniform sampler2D ssr;

uniform int motionBlurSamples;
uniform float velocityScale;

out vec4 fragColor;

vec3 FXAA(sampler2D sampleTexture, vec2 coord);
vec3 MotionBlur(vec3 color);
vec3 Bloom();
vec3 SSR();
vec3 ExposureTonemap(vec3 color);
vec3 GammaCorrect(vec3 color);

void main() {
    vec3 color = texture(screenTexture, texCoord).rgb;

    //FXAA
    if(useFXAA) color = FXAA(screenTexture, texCoord);

    //Motion Blur
    if(useMotionBlur) color = MotionBlur(color.rgb);

    //Bloom
    if(useBloom) color += Bloom();

    //SSR
    if(useSSR) color += SSR();

    //hdr tonemap
    color = ExposureTonemap(color);

    //gamma correct
    color = GammaCorrect(color);
    
    fragColor = vec4(color, 1.0f);

    if(ssrDebug && useSSR) fragColor = vec4(SSR(), 1.0f);
}

vec3 FXAA(sampler2D sampleTexture, vec2 coord) {
    //information from: https://www.youtube.com/watch?v=Z9bYzpwVINA

    //vector to determine luminosity of a pixel
    vec3 luma = vec3(0.299f, 0.587f, 0.114f);

    //sample in a cross pattern and calculate the luminosity of each sampled pixel
    float lumaM = dot(luma, texture(sampleTexture, coord).rgb); //middle
    float lumaTL = dot(luma, texture(sampleTexture, coord + vec2(-1.0f, -1.0f) * inverseScreenTextureSize).rgb); //top left
    float lumaTR = dot(luma, texture(sampleTexture, coord + vec2(1.0f, -1.0f) * inverseScreenTextureSize).rgb); //top right
    float lumaBL = dot(luma, texture(sampleTexture, coord + vec2(-1.0f, 1.0f) * inverseScreenTextureSize).rgb); //bottom left
    float lumaBR = dot(luma, texture(sampleTexture, coord + vec2(1.0f, 1.0f) * inverseScreenTextureSize).rgb); //bottom right

    //determine blur direction and detect edges (if the vectors x and y values are both 0, there is no edge)
    vec2 blurDirection = vec2(-((lumaTL + lumaTR) - (lumaBL + lumaBR)), (lumaTL + lumaBL) - (lumaTR + lumaBR));

    float directionReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * 0.25f * fxaaReduceMul, fxaaReduceMin); //prevent divsion by 0
    float inverseDirectionAdjustment = 1.0f / min(abs(blurDirection.x), abs(blurDirection.y)) + directionReduce; //determine smallest component

    //clamp blur direction to maximum blur range and convert to texel space
    blurDirection = min(vec2(fxaaSpanMax, fxaaSpanMax), 
                    max(vec2(-fxaaSpanMax, -fxaaSpanMax), blurDirection * inverseDirectionAdjustment)) * inverseScreenTextureSize; 

    //blur the edge pixels by sampling from the screen texture offset by the blur direction
    vec3 result1 =  (1.0f / 2.0f) * ( //smaller range
                    texture(sampleTexture, coord + (blurDirection * vec2(1.0f / 3.0f - 0.5f))).rgb +
                    texture(sampleTexture, coord + (blurDirection * vec2(2.0f / 3.0f - 0.5f))).rgb);

    vec3 result2 =  result1 * (1.0f / 2.0f) + (1.0f / 4.0f) * ( //bigger range
                    texture(sampleTexture, coord + (blurDirection * vec2(0.0f / 3.0f - 0.5f))).rgb +
                    texture(sampleTexture, coord + (blurDirection * vec2(3.0f / 3.0f - 0.5f))).rgb);

    //check if we sampled too far
    float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
    float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
    float lumaResult2 = dot(luma, result2);

    //return result1 if we sampled to far, otherwise return result2
    if(lumaResult2 < lumaMin || lumaResult2 > lumaMax) return result1;
    else return result2;
}

vec3 MotionBlur(vec3 color) {
    //information from: https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch27.html

    //obtain world position
    float zOverW = texture(sceneDepth, texCoord).r * 2.0f - 1.0f; //get the depth value
    vec4 H = vec4(texCoord * 2.0f - 1.0f, zOverW, 1.0f); //viewport position in the range [-1, 1]
    mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
    vec4 D = inverse(viewProjectionMatrix) * H; //transform by the view projection inverse matrix
    vec4 worldPos = D / D.w; //divide by w to get the world position

    //compute velocity vector
    vec4 currentPos = H; //current viewport position
    vec4 previousPos = previousViewProjectionMatrix * worldPos; //transform world position by the previous view projection matrix
    previousPos /= previousPos.w; //convert to nonhomogeneous points [-1, 1] by dividing by w
    vec2 velocity = (currentPos.xy - previousPos.xy) / 2.0f; //calculate pixel velocity
    velocity *= velocityScale;

    //blur the screen texture
    vec3 result = color;
    vec2 vTexCoord = texCoord - velocity; //sample along the velocity

    for(int i = 0; i < motionBlurSamples; ++i) {
        vec3 blurColor = texture(screenTexture, vTexCoord).rgb;
        
        if(useFXAA) blurColor = FXAA(screenTexture, vTexCoord); //also apply fxaa if it is enabled

        result += blurColor;
        vTexCoord -= velocity;
    }

    //average the results
    result /= (motionBlurSamples + 1.0f);

    return result;
}

vec3 Bloom() {
    return texture(bloomBlur, texCoord).rgb;
}

vec3 SSR() {
    vec3 color;

    if(useFXAA) {
        color = FXAA(ssr, texCoord); //apply FXAA as well so the blending doesnt look as obvious as FXAA blurs it a bit
    } else {
        color = texture(ssr, texCoord).rgb;
    }

    return color;
}

vec3 ExposureTonemap(vec3 color) {
    return vec3(1.0f) - exp(-color * exposure);
}

vec3 GammaCorrect(vec3 color) {
    return pow(color, vec3(1.0f / gamma));
}