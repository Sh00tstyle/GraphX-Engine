#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragPos;
out vec3 fragNormal;

void main() {
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.0f);
    fragPos = vec3(modelMatrix * vec4(vertex, 1.0f)); //get world space position
    //fragNormal = mat3(transpose(inverse(modelMatrix))) * normal; //applies the normal matrix, fixes non uniform scale for lighting
    fragNormal = vec3(modelMatrix * vec4(normal, 0.0f));

    //NOTE:
    //view space (eye position is always at (0, 0, 0))
    //fragPos = vec3(viewMatrix * modelMatrix * vec4(vertex, 1.0f));
    //fragNormal = vec3(viewMatrix * modelMatrix * vec4(vertex, 1.0f));
    //light pos would have to be in view space as well (do in cpu and send as uniform)
}