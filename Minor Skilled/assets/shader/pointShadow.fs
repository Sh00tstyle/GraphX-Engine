#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthCubemap;

uniform float farPlane;
uniform bool useShadows;

uniform vec3 lightPos;
uniform vec3 cameraPos;

out vec4 fragColor;

vec3 sampleOffsetDirections[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);  

float shadowCalculation() {
    vec3 fragToLight = fs_in.fragPos - lightPos; //use the direction vector to sample from the cubemap
    float closestDepth = texture(depthCubemap, fragToLight).x;
    closestDepth *= farPlane; //transform back to range [0, farPlane] (since the sample is in range [0, 1])
    float currentDepth = length(fragToLight); //get current linear depth between fragment and light position

    //test for shadows and apply bias
    float bias = 0.05f; //based on context, might need tweaking in other scenarios
    float shadow = 0.0f;

    //apply pcf with a third dimension
    int samples = 20;
    float offset = 0.1f;
    float cameraDistance = length(cameraPos - fs_in.fragPos);
    float diskRadius = (1.0f + (cameraDistance / farPlane)) / 25.0f; //radius to sample around the original fragToLight, based on the distance to the fragment

    for(int i = 0; i < samples; ++i) {
        float closestDepth = texture(depthCubemap, fragToLight + sampleOffsetDirections[i] * diskRadius).x;
        closestDepth *= farPlane;   // Undo mapping [0;1]

        if(currentDepth - bias > closestDepth) shadow += 1.0; //test for shadows
    }

    shadow /= float(samples);  

    //display the cubemap to debug
    //fragColor = vec4(vec3(closestDepth / farPlane), 1.0f); 

    return shadow;
}

void main() {
    vec3 color = texture(diffuseTexture, fs_in.texCoord).rgb;
    vec3 normal = normalize(fs_in.fragNormal);
    vec3 lightColor = vec3(0.3f);

    //ambient
    vec3 ambient = 0.3f * color;

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
    float shadow = useShadows ? shadowCalculation() : 0.0f;
    vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * color; //leave ambient out of the shadow calculation, so that we always have a bit of light

    fragColor = vec4(lighting, 1.0f);
}