#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec2 texCoord;
    vec3 tangentLightPos;
    vec3 tangentViewPos;
    vec3 tangentFragPos;
} fs_in;

//uniform sampler2D diffuseMap;
//uniform sampler2D normalMap;

uniform sampler2D textureDiffuse1;
uniform sampler2D textureNormal1;

out vec4 fragColor;

void main()
{           
    //obtain normal from normal map in range [0,1]
    vec3 normal = texture(textureNormal1, fs_in.texCoord).rgb;
    normal = normalize(normal * 2.0f - 1.0f);  //transform normal vector to range [-1,1] (tangent space)
   
    //get diffuse color
    vec3 color = texture(textureDiffuse1, fs_in.texCoord).rgb;

    //ambient
    vec3 ambient = 0.1f * color;

    //diffuse
    vec3 lightDirection = normalize(fs_in.tangentLightPos - fs_in.tangentFragPos);
    float difference = max(dot(lightDirection, normal), 0.0f);
    vec3 diffuse = difference * color;

    //specular
    vec3 lookDirection = normalize(fs_in.tangentViewPos - fs_in.tangentFragPos);
    vec3 halfwayDirection = normalize(lightDirection + lookDirection);  
    float specularity = pow(max(dot(normal, halfwayDirection), 0.0f), 32.0f);
    vec3 specular = vec3(0.2f) * specularity;

    fragColor = vec4(ambient + diffuse + specular, 1.0f);
}