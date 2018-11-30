#version 460 core

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
    //get distance between light and fragment
    float lightDistance = length(fragPos.xyz - lightPos);

    //map to range [0, 1] by deviding by the far plane
    lightDistance = lightDistance / farPlane;

    //output modified depth
    gl_FragDepth = lightDistance;
}