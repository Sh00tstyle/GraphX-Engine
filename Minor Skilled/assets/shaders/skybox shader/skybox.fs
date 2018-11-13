#version 330 core

in vec3 texCoord;

uniform samplerCube skybox;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main() {
    fragColor = texture(skybox, texCoord);
    brightColor = vec4(vec3(0.0f), 1.0f); //return black so it does not contribute to the bloom effect
}