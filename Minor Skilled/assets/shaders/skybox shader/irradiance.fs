#version 460 core

const float PI = 3.14159265359f;

in vec3 fragPos;

uniform samplerCube environmentMap;

out vec4 fragColor;

void main() {
    //acts as the normal of a tangent surface
    vec3 N = normalize(fragPos);

    vec3 irradiance = vec3(0.0f);

    //tangent space calculation from origin point
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 right = cross(up, N);
    up = cross(N, right);

    float sampleDelta = 0.025f;
    float sampleAmount = 0.0f;

    for(float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta) {
        for(float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta) {
            //spherical to cartesian in tangent space
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

            //from tangent to world space
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            sampleAmount += 1.0f;
        }
    }

    irradiance *= PI * (1.0f / sampleAmount);

    fragColor = vec4(irradiance, 1.0f);
}