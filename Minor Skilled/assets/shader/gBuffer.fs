#version 330 core

in VS_OUT {
    vec3 fragPosition;
    vec3 fragNormal;
    vec2 texCoord;
} fs_in;

uniform sampler2D textureDiffuse1;
uniform sampler2D textureSpecular1;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpecular;

void main() {
    //store position in the gPosition color buffer
    gPosition = fs_in.fragPosition;

    //store normal in the gNormal color buffer
    gNormal = fs_in.fragNormal;

    //store diffuse color in rgb of the gAlbedoSpecular color buffer
    gAlbedoSpecular.rgb = texture(textureDiffuse1, fs_in.texCoord).rgb;

    //store specular intensity in alpha of the gAlbedoSpecular color buffer
    gAlbedoSpecular.a = texture(textureSpecular1, fs_in.texCoord).r;
}