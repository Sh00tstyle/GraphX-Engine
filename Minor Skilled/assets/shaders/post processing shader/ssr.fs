#version 460 core

in vec2 texCoord;

layout (std140) uniform matricesBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 previousViewProjectionMatrix;
    mat4 lightSpaceMatrix;
};

uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D sceneTexture;
uniform sampler2D depthTexture;

uniform float rayStepSize;
uniform float maxRaySteps;

out vec4 fragColor;

vec3 SSR(vec3 position, vec3 normal);
vec4 ViewPosFromDepth(vec2 coord);

void main() {
    //information from: https://github.com/christoskaramou/VulkanMonkey3D/blob/master/VulkanMonkey/shaders/SSR/ssr.frag

    //get values from textures
    float specular = texture(gAlbedoSpec, texCoord).a;
    vec3 viewNormal = texture(gNormal, texCoord).xyz;
    vec3 viewPos = ViewPosFromDepth(texCoord).xyz;

    if(specular == 0.0f) {
        fragColor = vec4(0.0f);
        return;
    }

    fragColor = vec4(SSR(viewPos, viewNormal) * specular, 1.0f);
}

vec3 SSR(vec3 position, vec3 normal) {
    vec3 reflection = normalize(reflect(position, normalize(normal))); //reflect view direction over the surface normal

    float VdotR = max(dot(normalize(position), normalize(reflection)), 0.0f); //angle between the view direction and the reflection
    float fresnel = pow(VdotR, 5.0f); //fake fresnel

    //check for intersections with the depth buffer
    vec3 rayStep = reflection * rayStepSize;
    vec3 newPos = position + rayStep;

    float loops = max(sign(VdotR), 0.0f) * maxRaySteps;

    for(int i = 0; i < maxRaySteps; i++) {
        vec4 newViewPos = vec4(newPos, 1.0f); //current view space position

        vec4 samplePos = projectionMatrix * newViewPos; //transform to clip space
        samplePos.xy /= samplePos.w; //perform perspective divide
        samplePos.xy = samplePos.xy * 0.5f + 0.5f; //bring from range [-1, 1] to range [0, 1]

        vec2 checkBoundsUV = max(sign(samplePos.xy * (1.0f - samplePos.xy)), 0.0f);

        if(checkBoundsUV.x * checkBoundsUV.y < 1.0f) {
            rayStep *= 0.5f;
            newPos -= rayStep;

            continue;
        }

        float currentDepth = abs(newViewPos.z); //current depth
        float sampledDepth = abs(ViewPosFromDepth(samplePos.xy).z); //get depth from the texture

        float delta = abs(currentDepth - sampledDepth); //check for the distance between the ray depth and the depth buffer

        if(delta < 0.03f) { //intersection found
            //fade out towards the edges to avoid showing artifacts
            vec2 dCoords = smoothstep(0.2f, 0.6f, abs(vec2(0.5f, 0.5f) - samplePos.xy));
            float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

            return texture(sceneTexture, samplePos.xy).rgb * fresnel * screenEdgefactor; //sample the color and apply fresnel and fade
        }

        rayStep *= 1.0f - 0.5f * max(sign(currentDepth - sampledDepth), 0.0f);
        newPos += rayStep * (sign(sampledDepth - currentDepth) + 0.000001f);
    }

    return vec3(0.0f); //no hit found
}

vec4 ViewPosFromDepth(vec2 coord) {
    float depth = texture(depthTexture, coord).r; //get raw depth

    vec4 clipSpacePos = vec4(texCoord, depth, 1.0f); //construct raw position
    clipSpacePos.xyz = clipSpacePos.xyz * 2.0f - 1.0f; //bring from range [0, 1] to range [-1, 1]

    vec4 viewSpacePos = inverse(projectionMatrix) * clipSpacePos; //transform to view space
    viewSpacePos.xyz /= viewSpacePos.w; //perform perspective divide

    return viewSpacePos;
}