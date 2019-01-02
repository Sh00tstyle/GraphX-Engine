#version 460 core

in vec2 texCoord;

layout (std140) uniform matricesBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 previousViewProjectionMatrix;
    mat4 lightSpaceMatrix;
};

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;

uniform vec3 samples[64];

uniform float screenWidth;
uniform float screenHeight;

//ssao parameters
uniform int kernelSize;
uniform float radius;
uniform float bias;
uniform float power;

out float fragColor;

void main() {
    vec2 noiseScale = vec2(screenWidth / 4.0f, screenHeight / 4.0f);

    //get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, texCoord).xyz;
    vec3 normal = normalize(texture(gNormal, texCoord).xyz);
    vec3 randomVec = normalize(texture(noiseTexture, texCoord * noiseScale).xyz);

    //create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    //iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0f;

    for(int i = 0; i < kernelSize; i++) {
        //get sample position
        vec3 samplePos = TBN * samples[i]; //from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        //project sample position to sample texture to get position on screen/texture
        vec4 offset = vec4(samplePos, 1.0f);
        offset = projectionMatrix * offset; //from view to clip-space
        offset.xyz /= offset.w; //perspective divide
        offset.xyz = offset.xyz * 0.5f + 0.5f; //transform to range 0.0 - 1.0
        
        //get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; //get depth value of kernel sample
        
        //range check & accumulate
        float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0f : 0.0f) * rangeCheck;           
    }

    occlusion = 1.0f - (occlusion / kernelSize);
    
    fragColor = pow(occlusion, power);
}