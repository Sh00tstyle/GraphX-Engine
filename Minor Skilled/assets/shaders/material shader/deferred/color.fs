#version 460 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    float specular;
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
layout (location = 4) out vec3 gEnvironment;

void main() {
    //store the data in the gBuffer
    gPosition.rgb = fs_in.fragPos;

    gNormal.rgb = normalize(fs_in.fragNormal);

    gAlbedoSpec.rgb = material.diffuse;
    gAlbedoSpec.a = material.specular;
    
    gEmissionShiny.rgb = vec3(0.0f);
    gEmissionShiny.a = material.shininess / 255.0f;

    gEnvironment.rgb = vec3(0.0f);
}