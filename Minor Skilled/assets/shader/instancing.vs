#version 330 core
/**
layout (location = 0) in vec2 aVertex;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

uniform vec2 offsets[100]; //offsets for 100 different instances

out vec3 fColor;

void main() {
    //vec2 offset = offsets[gl_InstanceID];

    vec2 pos = aVertex * (gl_InstanceID / 100.0f); //scale 
    gl_Position = vec4(pos + aOffset, 0.0f, 1.0f);
    fColor = aColor;
}
/**/

layout (location = 0) in vec3 aVertex;
layout (location = 2) in vec2 aUV;
layout (location = 3) in mat4 instanceMatrix;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 texCoord;

void main() {
    gl_Position = projectionMatrix * viewMatrix * instanceMatrix * vec4(aVertex, 1.0f);
    texCoord = aUV;
}