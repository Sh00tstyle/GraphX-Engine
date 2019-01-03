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
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec2 gEmissionSpec;
layout (location = 4) out vec4 gEnvironmentShiny;

void main() {
    //store the data in the gBuffer
    gPosition.rgb = fs_in.fragPos;

    gNormal.rgb = normalize(fs_in.fragNormal);

    gAlbedo.rgb = material.diffuse;
    
    gEmissionSpec.r = 0.0f;
    gEmissionSpec.g = material.specular;

    gEnvironmentShiny.rgb = vec3(0.0f);
    gEnvironmentShiny.a = material.shininess / 255.0f;
}