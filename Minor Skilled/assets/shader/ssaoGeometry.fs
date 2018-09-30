#version 330 core

in vec2 texCoord;
in vec3 fragPos;
in vec3 fragNormal;

uniform sampler2D textureDiffuse1;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

void main() {    
    //store the fragment position vector in the first gbuffer texture
    gPosition = fragPos;

    //also store the per-fragment normals into the gbuffer
    gNormal = normalize(fragNormal);

    //and the diffuse per-fragment color
    gAlbedo.rgb = vec3(0.95f);
    //gAlbedo.rgb = texture(textureDiffuse1, texCoord).rgb;
}