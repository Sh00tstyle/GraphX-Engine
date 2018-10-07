#version 330 core

in vec3 textureDir;

uniform samplerCube cubemap;

out vec4 fragColor;

void main() {
    vec3 color = texture(cubemap, textureDir).rgb;

    //HDR tonemap and gamma correct
    color = color / (color + vec3(1.0f));
    color = pow(color, vec3(1.0f/2.2f));

    fragColor = vec4(color, 1.0f);
}