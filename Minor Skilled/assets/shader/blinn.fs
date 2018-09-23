#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 fragNormal;
    vec2 texCoord;
} fs_in;

uniform sampler2D floorTexture;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform bool useBlinn;

out vec4 fragColor;

void main() {
    vec3 color = texture(floorTexture, fs_in.texCoord).rgb;

    //ambient
    vec3 ambient = 0.05f * color; //ambient light color of vec3(0.05, 0.05, 0.05) - dark grey

    //diffuse
    vec3 lightDirection = normalize(lightPos - fs_in.fragPos);
    vec3 normal = normalize(fs_in.fragNormal);
    float difference = max(dot(lightDirection, normal), 0.0f);
    vec3 diffuse = difference * color; //diffuse light color of vec3(1, 1, 1) - white

    //specular
    vec3 lookDirection = normalize(cameraPos - fs_in.fragPos);
    float specularity = 0.0f;

    if(useBlinn) {
        vec3 halfwayDir = normalize(lightDirection + lookDirection);
        specularity = pow(max(dot(normal, halfwayDir), 0.0), 32.0); //uses the halfway direction instead of the reflect direction
        //shininess for blinn should be between 2 - 4 times the shininess of phing to get similar results
    } else {
        vec3 reflectDirection = reflect(-lightDirection, normal);
        specularity = pow(max(dot(normal, reflectDirection), 0.0), 8.0); //uses the usual reflect direction
    }

    vec3 specular = vec3(0.3f) * specularity; //specular light color of vec3(1, 1, 1) - white
    //vec3 specular = vec3(0.3f) * specularity * color; //applying the texture color as well

    fragColor = vec4(ambient + diffuse + specular, 1.0f);
}