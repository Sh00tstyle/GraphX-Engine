#version 460 core

void main() {
    gl_FragDepth = gl_FragCoord.z; //only output the depth (shader could also be left blank)
}