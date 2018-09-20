#version 330 core

in vec3 textureDir;

uniform samplerCube cubemap;

out vec4 fragColor;

void main() {
    fragColor = texture(cubemap, textureDir);
}