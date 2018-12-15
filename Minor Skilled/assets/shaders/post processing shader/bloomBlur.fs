#version 460 core

in vec2 texCoord;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270f, 0.1945945946f, 0.1216216216f, 0.0540540541f, 0.0162162162f);

out vec4 fragColor;

void main() {
    vec2 texelOffset = 1.0f / textureSize(image, 0); //gets size of single texel
    vec3 result = texture(image, texCoord).rgb * weight[0];

    if(horizontal) {
        for(int i = 1; i < 5; ++i) {
            result += texture(image, texCoord + vec2(texelOffset.x * i, 0.0f)).rgb * weight[i];
            result += texture(image, texCoord - vec2(texelOffset.x * i, 0.0f)).rgb * weight[i];
        }
    } else {
        for(int i = 1; i < 5; ++i) {
            result += texture(image, texCoord + vec2(0.0f, texelOffset.y * i)).rgb * weight[i];
            result += texture(image, texCoord - vec2(0.0f, texelOffset.y * i)).rgb * weight[i];
        }
    }

    float fragDepth = texture(image, texCoord).a; //forward the depth values
    fragColor = vec4(result, fragDepth);
}