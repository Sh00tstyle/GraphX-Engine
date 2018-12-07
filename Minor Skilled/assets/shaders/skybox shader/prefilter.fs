#version 460 core

const float PI = 3.14159265359f;
const uint SAMPLES = 1024u;

in vec3 fragPos;

uniform float roughness;
uniform float resolution;
uniform samplerCube environmentMap;

out vec4 fragColor;

float DistributionGGX(vec3 N, vec3 H);
float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N);

void main() {
    vec3 N = normalize(fragPos);

    //assume that V equals R equals the normal
    vec3 R = N;
    vec3 V = R;

    vec3 prefilteredColor = vec3(0.0f);
    float totalWeight = 0.0f;

    for(uint i = 0u; i < SAMPLES; ++i) {
        //generates a sample vector biased towards the preferred alignment direction (importance sampling)
        vec2 Xi = Hammersley(i, SAMPLES);
        vec3 H = ImportanceSampleGGX(Xi, N);
        vec3 L = normalize(2.0f * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0f);

        if(NdotL > 0.0f) {
            //sample from the environment's mip level based on roughness/pdf
            float D = DistributionGGX(N, H);
            float NdotH = max(dot(N, H), 0.0f);
            float HdotV = max(dot(H, V), 0.0f);
            float pdf = D * NdotH / (4.0f * HdotV) + 0.0001f; 

            float saTexel  = 4.0f * PI / (6.0f * resolution * resolution);
            float saSample = 1.0f / (float(SAMPLES) * pdf + 0.0001f);

            float mipLevel = roughness == 0.0f ? 0.0f : 0.5f * log2(saSample / saTexel); 
            
            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    fragColor = vec4(prefilteredColor, 1.0f);
}

float DistributionGGX(vec3 N, vec3 H) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return num / denom;
}

float RadicalInverse_VdC(uint bits) {
    //http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
    //efficient VanDerCorpus calculation.
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N) {
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N) {
    float a = roughness * roughness;

    float phi = 2.0f * PI * Xi.x;
    float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

    //from spherical to cartesian coordinates - halfway vector
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    //from tangent space H vector to world space sample vector
    vec3 up = abs(N.z) < 0.999 ? vec3(0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}