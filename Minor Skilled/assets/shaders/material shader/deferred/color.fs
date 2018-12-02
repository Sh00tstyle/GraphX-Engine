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

layout (location = 0) out vec4 gPositionRefract;
layout (location = 1) out vec4 gNormalReflect;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gEmissionShiny;

void main() {
    //store the data in the gBuffer
    gPositionRefract.rgb = fs_in.fragPos;
    gPositionRefract.a = 0.0f;

    gNormalReflect.rgb = normalize(fs_in.fragNormal);
    gNormalReflect.a = 0.0f;

    gAlbedoSpec.rgb = material.diffuse;
    gAlbedoSpec.a = material.specular.r;
    
    gEmissionShiny.rgb = vec3(0.0f);
    gEmissionShiny.a = material.shininess;
}