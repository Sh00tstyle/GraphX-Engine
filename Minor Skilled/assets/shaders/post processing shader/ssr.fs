#version 460 core

//jitter
const vec3 scale = vec3(0.8f);
const float K = 19.19f;

in vec2 texCoord;

layout (std140) uniform matricesBlock {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 previousViewProjectionMatrix;
    mat4 lightSpaceMatrix;
};

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D sceneTexture;
uniform sampler2D depthTexture;

uniform float rayStep;
uniform float minRayStep;
uniform int maxSteps;
uniform int binarySearchStepAmount;
uniform float reflectionSpecularFalloffExponent;
uniform float maxThickness;

uniform float maxDepth;

out vec4 fragColor;

vec3 BinarySearch(vec3 direction, inout vec3 hitCoord, out float dDepth);
vec4 RayCast(vec3 direction, inout vec3 hitCoord, out float dDepth);
vec3 Hash(vec3 a);

void main() {
    //information from: http://imanolfotia.com/blog/update/2017/03/11/ScreenSpaceReflections.html

    //get values from textures
    float specular = texture(gAlbedoSpec, texCoord).a;

    if(specular == 0.0f) {
        //do not check for reflections if the surface is not glossy
        fragColor = vec4(0.0f);
        return;
    }

    vec3 viewNormal = texture(gNormal, texCoord).xyz;
    vec3 viewPos = texture(gPosition, texCoord).xyz;

    //reflect view direction over the surface normal in view space
    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));

    //check for intersections with the depth buffer by ray casting
    vec3 hitPos = viewPos;
    float dDepth; //delta depth

    vec3 jitter = mix(vec3(0.0f), vec3(Hash(viewPos)), specular);
    jitter = vec3(0.0f);
    vec4 coord = RayCast(jitter + reflected * max(minRayStep, -viewPos.z), hitPos, dDepth);
    vec2 dCoord = smoothstep(0.2f, 0.6f, abs(vec2(0.5f, 0.5f) - coord.xy));

    float screenEdgefactor = clamp(1.0f - (dCoord.x + dCoord.y), 0.0f, 1.0f);
    float reflectionMultiplier = pow(specular, reflectionSpecularFalloffExponent) * screenEdgefactor * -reflected.z;

    //get color from hit position
    vec3 SSR = texture(sceneTexture, coord.xy).rgb * clamp(reflectionMultiplier, 0.0f, 0.9f);

    fragColor = vec4(SSR, specular);
}

vec3 BinarySearch(vec3 direction, inout vec3 hitCoord, out float dDepth) {
    float depth;
    vec4 projectedCoord;

    for(int i = 0; i < binarySearchStepAmount; i++) {
        //transform from view space to clip space in NDC
        projectedCoord = projectionMatrix * vec4(hitCoord, 1.0f);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5f + 0.5f;

        //check for intersection
        depth = texture(gPosition, projectedCoord.xy).z;
        dDepth = hitCoord.z - depth;

        direction *= 0.5f;

        if(dDepth > 0.0f) {
            hitCoord += direction;
        } else {
            hitCoord -= direction;
        }
    }

    projectedCoord = projectionMatrix * vec4(hitCoord, 1.0f);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5f + 0.5f;

    return vec3(projectedCoord.xy, depth);
}

vec4 RayCast(vec3 direction, inout vec3 hitCoord, out float dDepth) {
    direction *= rayStep;

    float depth;
    vec4 projectedCoord;

    for(int i = 0; i < maxSteps; i++) {
        hitCoord += direction;

        //transform from view space to clip space in NDC
        projectedCoord = projectionMatrix * vec4(hitCoord, 1.0f);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5f + 0.5f;

        //check for intersection
        depth = texture(gPosition, projectedCoord.xy).z;

        if(depth > maxDepth) continue;

        dDepth = hitCoord.z - depth;

        if((direction.z - dDepth) < maxThickness && dDepth <= 0.0f) {
            vec4 result = vec4(BinarySearch(direction, hitCoord, dDepth), 1.0f);

            return result;
        }
    }

    return vec4(projectedCoord.xy, depth, 0.0f);
}

vec3 Hash(vec3 a) {
    a = fract(a * scale);
    a += dot(a, a.yxz + K);

    return fract((a.xxy + a.yxx) * a.zyx);
}