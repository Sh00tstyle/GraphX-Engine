#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out; //output 6 triangles

uniform mat4 shadowMatrices[6]; //one for each face

out vec4 fragPos;

void main() {
    for(int face = 0; face < 6; ++face) {
        gl_Layer = face; //built in variable which specified to which cubemap face we render

        for(int i = 0; i < 3; ++i) { //for each vertex of a triangle
            fragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * fragPos; //transform to light space
            EmitVertex();
        }

        EndPrimitive();
    }
}