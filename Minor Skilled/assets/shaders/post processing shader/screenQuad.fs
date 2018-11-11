#version 330 core

in vec2 texCoord;

uniform sampler2D screenTexture;

out vec4 fragColor;

void main() {
    float depthValue = texture(screenTexture, texCoord).r;
    fragColor = vec4(vec3(depthValue), 1.0f);

    //fragColor = vec4(texture(screenTexture, texCoord).rgb, 1.0f);
}