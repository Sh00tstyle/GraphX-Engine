#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
    vec4 fragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D depthTexture;

uniform vec3 lightPos;
uniform vec3 cameraPos;

out vec4 fragColor;

float shadowCalculation(vec3 normal, vec3 lightDirection) {
    //transform to NDC (only needed for perspective, not for orthographic)
    vec3 projectedCoord = fs_in.fragPosLightSpace.xyz / fs_in.fragPosLightSpace.w;
    //transform to range [0, 1], since the depth texture is also in this range
    projectedCoord = projectedCoord * 0.5f + 0.5f; 
    //get depth of closest fragment from lights perspective
    float closestDepth = texture(depthTexture, projectedCoord.xy).x;
    //get depth of current fragment from lights perspective
    float currentDepth = projectedCoord.z;
    //add bias to avoid shadow acne
    //float bias = 0.005f;
    float bias = max(0.05f * (1.0f - dot(normal, lightDirection)), 0.005f);
    //check if the current fragment is in a shadow
    float shadow = 0.0f;
    vec2 texelSize = 1.0f / textureSize(depthTexture, 0); //width and height at mipmap level 0

    //apply pcf (percentage-closer filtering), sample from different coords and average it to get smoother shadows
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(depthTexture, projectedCoord.xy + vec2(x, y) * texelSize).x; //sample with offset
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }

    shadow /= 9.0f;

    //fix dark areas that are outside of the lights projection (everything outside of the depth map has no visible shadows)
    if(projectedCoord.z > 1.0f) shadow = 0.0f;

    return shadow;
}

void main() {
    vec3 color = texture(diffuseTexture, fs_in.texCoord).rgb;
    vec3 normal = normalize(fs_in.fragNormal);
    vec3 lightColor = vec3(1.0f);

    //ambient
    vec3 ambient = 0.15f * color;

    //diffuse
    vec3 lightDirection = normalize(lightPos - fs_in.fragPos);
    float difference = max(dot(lightDirection, normal), 0.0f);
    vec3 diffuse = difference * lightColor;

    //specular
    vec3 lookDirection = normalize(cameraPos - fs_in.fragPos);
    vec3 halfwayDirection = normalize(lightDirection + lookDirection);
    float specularity = pow(max(dot(normal, halfwayDirection), 0.0f), 64.0f);
    vec3 specular = specularity * lightColor;

    //calcualte shadow
    float shadow = shadowCalculation(normal, lightDirection);
    vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * color; //leave ambient out of the shadow calculation, so that we always have a bit of light

    fragColor = vec4(lighting, 1.0f);
}