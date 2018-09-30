#version 330 core
layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

uniform bool invertedNormals;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 texCoord;

void main() {
    vec4 viewPos = viewMatrix * modelMatrix * vec4(aVertex, 1.0f);
    fragPos = viewPos.xyz; 
    texCoord = aUV;
    
    mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));
    fragNormal = normalMatrix * (invertedNormals ? -aNormal : aNormal);
    
    gl_Position = projectionMatrix * viewPos;
}