#version 330 core
layout (location = 0) in vec3 aVertex;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent; //not really needed, since we can just calculate it ourself

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 lightPos;
uniform vec3 cameraPos;

out VS_OUT {
    vec3 fragPos;
    vec2 texCoord;
    vec3 tangentLightPos;
    vec3 tangentViewPos;
    vec3 tangentFragPos;
} vs_out;

void main()
{
    vs_out.fragPos = vec3(modelMatrix * vec4(aVertex, 1.0));   
    vs_out.texCoord = aUV;

    vec3 tangent = normalize(mat3(modelMatrix) * aTangent);
    vec3 normal = normalize(mat3(modelMatrix) * aNormal);
    vec3 bitangent = cross(normal, tangent); //can also be calculated
    
    //convert to tangent space to avoid converting it in the fragment shader (since it runs more often)
    mat3 TBN = transpose(mat3(tangent, bitangent, normal));    
    vs_out.tangentLightPos = TBN * lightPos;
    vs_out.tangentViewPos  = TBN * cameraPos;
    vs_out.tangentFragPos  = TBN * vs_out.fragPos;
        
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aVertex, 1.0);
}