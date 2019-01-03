#version 460 core

in vec2 texCoord;

layout (std140) uniform matricesBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 previousViewProjectionMatrix;
    mat4 lightSpaceMatrix;
};

uniform sampler2D gNormal;
uniform sampler2D gEmissionSpec;
uniform sampler2D sceneTexture;
uniform sampler2D depthTexture;

uniform float rayStepSize;
uniform float maxRaySteps;
uniform float fresnelExponent;
uniform float maxDelta;

out vec4 fragColor;

vec3 SSR(vec3 position, vec3 normal);
vec4 ViewPosFromDepth(vec2 coord);
vec3 FresnelSchlick(float cosTheta, vec3 F0);

void main() {
    //information from: https://github.com/christoskaramou/VulkanMonkey3D/blob/master/VulkanMonkey/shaders/SSR/ssr.frag

    //get values from textures
    float specular = texture(gEmissionSpec, texCoord).g;
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
    float fresnel = pow(VdotR, fresnelExponent); //simplified fresnel approximation

    //check for intersections with the depth buffer
    vec3 rayStep = reflection * rayStepSize;
    vec3 newPos = position + rayStep;

    for(int i = 0; i < maxRaySteps; i++) {
        vec4 newViewPos = vec4(newPos, 1.0f); //current view space position

        vec4 samplePos = projectionMatrix * newViewPos; //transform to clip space
        samplePos.xy /= samplePos.w; //perform perspective divide
        samplePos.xy = samplePos.xy * 0.5f + 0.5f; //bring from range [-1, 1] to range [0, 1]

        //check if we are outside of the UV range 
        vec2 checkBoundsUV = max(sign(samplePos.xy * (1.0f - samplePos.xy)), 0.0f);

        if(checkBoundsUV.x * checkBoundsUV.y < 1.0f) {
            //we are outside the uv range so half the ray step, step backwards and try again
            rayStep *= 0.5f;
            newPos -= rayStep;

            continue;
        }

        float currentDepth = abs(newViewPos.z); //current ray depth
        float sampledDepth = abs(ViewPosFromDepth(samplePos.xy).z); //depth from the depth buffer

        float delta = abs(currentDepth - sampledDepth); //check for the distance between the ray depth and the depth buffer

        if(delta < maxDelta) { //intersection found
            //fade out towards the edges to avoid showing artifacts
            vec2 dCoords = smoothstep(0.2f, 0.6f, abs(vec2(0.5f, 0.5f) - samplePos.xy));
            float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

            return texture(sceneTexture, samplePos.xy).rgb * fresnel * screenEdgefactor; //sample the color from the scene texture and apply fresnel and fade
        }

        rayStep *= 1.0f - 0.5f * max(sign(currentDepth - sampledDepth), 0.0f); //half the step size if the ray overshot the depth buffer by more than the delta threshold
        newPos += rayStep * (sign(sampledDepth - currentDepth) + 0.000001f); //step back on the ray if it overshot teh depth buffer, otherwise step further
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