#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
} fs_in;

uniform vec3 color;

out vec4 fragColor;

void main() {
    //TODO: lighting calculation

    fragColor = vec4(color, 1.0f);
}