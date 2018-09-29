#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec2 texCoord;
    vec3 tangentLightPos;
    vec3 tangentViewPos;
    vec3 tangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform float heightScale;

out vec4 fragColor;

vec2 ParallaxMapping(vec3 lookDirection) {
    //steep parallax mapping
    float minLayerAmount = 8.0f;
    float maxLayerAmount = 32.0f;
    float layerAmount = mix(maxLayerAmount, minLayerAmount, abs(dot(vec3(0.0f, 0.0f, 1.0f), lookDirection)));

    //calculate size of each layer
    float layerDepth = 1.0f / layerAmount;
    float currentLayerDepth = 0.0f;

    //amount to shift texture coordinates from vector p per layer
    vec2 p = lookDirection.xy * heightScale;
    vec2 deltaTexCoord = p / layerAmount;

    //initialize
    vec2 currentTexCoord = fs_in.texCoord;
    float currentDepthMapValue = texture(depthMap, currentTexCoord).x;

    //raycasting
    while(currentLayerDepth < currentDepthMapValue) {
        //shift uv along the direction of p
        currentTexCoord -= deltaTexCoord;

        //sample with offset uv
        currentDepthMapValue = texture(depthMap, currentTexCoord).x;
        
        //get depth of next layer
        currentLayerDepth += layerDepth;
    }

    //return currentTexCoord;

    //parallax occlusion mapping
    vec2 prevTexCoord = currentTexCoord + deltaTexCoord; //coordinates before collision

    //get depth from before and after collision to interpolate
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthMap, prevTexCoord).x - currentLayerDepth + layerDepth;

    //interpolation of texcood
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoord = prevTexCoord * weight + currentTexCoord * (1.0f - weight);

    return finalTexCoord;

    /**
    //parallax mapping
    float height = texture(depthMap, fs_in.texCoord).x;
    vec2 p = lookDirection.xy / lookDirection.z * (height * heightScale);

    return fs_in.texCoord - p;
    /**/
}

void main()
{          
    //offset uv with parallax mapping
    vec3 lookDirection = normalize(fs_in.tangentViewPos - fs_in.tangentFragPos);
    vec2 texCoord = ParallaxMapping(lookDirection);

    //discard tex coords out which are out of uv range
    if(texCoord.x > 1.0f || texCoord.y > 1.0f || texCoord.x < 0.0f || texCoord.y < 0.0f) discard;

    //obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, texCoord).rgb;
    normal = normalize(normal * 2.0f - 1.0f);  //transform normal vector to range [-1,1] (tangent space)
   
    //get diffuse color
    vec3 color = texture(diffuseMap, texCoord).rgb;

    //ambient
    vec3 ambient = 0.1f * color;

    //diffuse
    vec3 lightDirection = normalize(fs_in.tangentLightPos - fs_in.tangentFragPos);
    float difference = max(dot(lightDirection, normal), 0.0f);
    vec3 diffuse = difference * color;

    //specular
    vec3 halfwayDirection = normalize(lightDirection + lookDirection);  
    float specularity = pow(max(dot(normal, halfwayDirection), 0.0f), 32.0f);
    vec3 specular = vec3(0.2f) * specularity;

    fragColor = vec4(ambient + diffuse + specular, 1.0f);
}