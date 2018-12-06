#version 460 core

const vec2 invAtan = vec2(0.1591f, 0.3183f);

in vec3 fragPos;

uniform sampler2D equirectangularMap;

out vec4 fragColor;

vec2 SampleSphericalMap(vec3 v);

void main() {
    vec2 uv = SampleSphericalMap(normalize(fragPos));
    vec3 color = texture(equirectangularMap, uv).rgb;

    fragColor = vec4(color, 1.0f);
}

vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5f;

    return uv;
}