#version 460 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
} fs_in;

uniform Material material;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gEmissionShiny;

void main() {
    //store the data in the gBuffer
    gPosition = fs_in.fragPos;

    gNormal = normalize(fs_in.fragNormal);

    gAlbedoSpec.rgb = material.diffuse;
    gAlbedoSpec.a = material.specular.r;
    
    gEmissionShiny.rgb = vec3(0.0f);
    gEmissionShiny.a = material.shininess;
}