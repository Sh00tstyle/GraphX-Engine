#version 330 core

in vec2 texCoord;

uniform sampler2D textureDiffuse1;

out vec4 fragColor;

void main() {
    fragColor = texture(textureDiffuse1, texCoord);
}