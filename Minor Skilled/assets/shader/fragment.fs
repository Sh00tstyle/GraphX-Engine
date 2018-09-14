#version 330 core

in vec4 ourColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

out vec4 fragColor;

void main() {
    //fragColor = ourColor;
    //fragColor = texture(ourTexture, texCoord) * ourColor;
    fragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2f);
}