#version 330 core

in vec2 texCoord;

uniform mat4 projectionMatrix;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;

uniform vec3 samples[64];

//parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernelSize = 64;
float radius = 0.9f;
float bias = 0.025f;
float power = 5.0f;

//tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(1280.0f/4.0f, 720.0f/4.0f); 

out float fragColor;

void main() {
    //get input for SSAO algorithm
    vec3 fragPos = texture(gPosition, texCoord).xyz;
    vec3 normal = normalize(texture(gNormal, texCoord).rgb);
    vec3 randomVec = normalize(texture(noiseTexture, texCoord * noiseScale).xyz);

    //create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    //iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0f;

    for(int i = 0; i < kernelSize; i++) {
        //get sample position
        vec3 sample = TBN * samples[i]; // from tangent to view-space
        sample = fragPos + sample * radius; 
        
        //project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sample, 1.0f);
        offset = projectionMatrix * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5f + 0.5f; // transform to range 0.0 - 1.0
        
        //get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample
        
        //range check & accumulate
        float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0f : 0.0f) * rangeCheck;           
    }

    occlusion = 1.0f - (occlusion / kernelSize);
    occlusion = pow(occlusion, power);
    
    fragColor = occlusion;
}