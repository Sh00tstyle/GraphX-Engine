#version 330 core

in vec2 texCoord;

uniform sampler2D screenTexture;

out vec4 fragColor;

const float offset = 1.0f / 300.0f;

void main() {
    //fragColor = texture(screenTexture, texCoord);
    //fragColor = vec4(vec3(1.0f - texture(screenTexture, texCoord)), 1.0f); //inversion

    //vec3 texColor = texture(screenTexture, texCoord).xyz; 
    //float average = (texColor.r + texColor.g + texColor.b) / 3.0f;
    //fragColor = vec4(average, average, average, 1.0f); //grayscale

    //kernel effects
    vec2 offsets[9] = vec2[] (
        vec2(-offset, offset), //top-left
        vec2( 0.0f, offset), //top-center
        vec2( offset, offset), //top-right
        vec2(-offset, 0.0f),   //center-left
        vec2( 0.0f, 0.0f),   //center-center
        vec2( offset, 0.0f),   //center-right
        vec2(-offset, -offset), //bottom-left
        vec2( 0.0f, -offset), //bottom-center
        vec2( offset, -offset)  //bottom-right    
    );

    //sharpen kernel
    /**
    float kernel[9] = float[](
        -1, -1, -1,
        -1, 9, -1,
        -1, -1, -1
    );
    /**/

    //blur kernel
    /**
    float kernel[9] = float[](
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    );
    /**/

    //edge detection kernel
    float kernel[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    vec3 color = vec3(0.0f);

    for(int i = 0; i < 9; i++) {
        color += vec3(texture(screenTexture, texCoord + offsets[i])) * kernel[i];
    }

    fragColor = vec4(color, 1.0f);
}