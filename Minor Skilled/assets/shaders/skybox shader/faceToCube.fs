#version 460 core

in vec2 texCoord;

uniform sampler2D cubemapFaces[6];

layout (location = 0) out vec4 face1;
layout (location = 1) out vec4 face2;
layout (location = 2) out vec4 face3;
layout (location = 3) out vec4 face4;
layout (location = 4) out vec4 face5;
layout (location = 5) out vec4 face6;

void main() {
    face1 = vec4(texture(cubemapFaces[0], texCoord).rgb, 1.0f);
    face2 = vec4(texture(cubemapFaces[1], texCoord).rgb, 1.0f);
    face3 = vec4(texture(cubemapFaces[2], texCoord).rgb, 1.0f);
    face4 = vec4(texture(cubemapFaces[3], texCoord).rgb, 1.0f);
    face5 = vec4(texture(cubemapFaces[4], texCoord).rgb, 1.0f);
    face6 = vec4(texture(cubemapFaces[5], texCoord).rgb, 1.0f);
}