#version 330 core

void main() {
    gl_FragDepth = gl_FragCoord.z; //we only care about the depth (happens automatically, even if this is commented)
}