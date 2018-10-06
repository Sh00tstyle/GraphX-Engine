#version 330 core

in vec3 textureDir;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591f, 0.3183f);

out vec4 fragColor;

vec2 sampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5f;
    return uv;
}

void main() {		
    vec2 uv = sampleSphericalMap(normalize(textureDir));
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    fragColor = vec4(color, 1.0f);
}