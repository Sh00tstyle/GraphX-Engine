#version 330 core

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
    float lightDistance = length(fragPos.xyz - lightPos);

    //map to range [0, 1] by dividing by far plane
    lightDistance = lightDistance / farPlane;

    //write modified depth
    gl_FragDepth = lightDistance;
}