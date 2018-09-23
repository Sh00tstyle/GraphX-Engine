#version 330 core

/**
in vec3 fColor;

out vec4 fragColor;

void main() {
    fragColor = vec4(fColor, 1.0f);
}
/**/

in vec2 texCoord;

uniform sampler2D textureDiffuse1;

out vec4 fragColor;

void main() {
    fragColor = texture(textureDiffuse1, texCoord);
}