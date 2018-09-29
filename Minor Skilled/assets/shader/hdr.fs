#version 330 core

in vec2 texCoord;

uniform sampler2D hdrTexture;
uniform bool useHdr;
uniform float exposure;

out vec4 fragColor;

void main() {             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrTexture, texCoord).rgb;

    if(useHdr) {
        //reinhard
        //vec3 result = hdrColor / (hdrColor + vec3(1.0));

        //exposure
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

        //gamme correction    
        result = pow(result, vec3(1.0 / gamma));
        fragColor = vec4(result, 1.0);
    } else {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        fragColor = vec4(result, 1.0);
    }
}