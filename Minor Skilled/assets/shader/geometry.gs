//geometry shader
#version 330 core

/**
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

//could also just be written as
//in vec3 color[];

in VS_OUT {
    vec3 color;
} gs_in[]; //always represented as array of vertices in the geometry shader

out vec3 fColor;

void buildHouse(vec4 position) {
    fColor = gs_in[0].color; //index 0, since there is only one input vertex

    gl_Position = position + vec4(-0.2f, -0.2f, 0.0f, 0.0f); //1: bottom left
    EmitVertex();

    gl_Position = position + vec4(0.2f, -0.2f, 0.0f, 0.0f); //2: bottom right
    EmitVertex();

    gl_Position = position + vec4(-0.2f, 0.2f, 0.0f, 0.0f); //3: top left
    EmitVertex();

    gl_Position = position + vec4(0.2f, 0.2f, 0.0f, 0.0f); //4: top right
    EmitVertex();

    gl_Position = position + vec4(0.0f, 0.4f, 0.0f, 0.0f); //5: top
    fColor = vec3(1.0f, 1.0f, 1.0f); //give the last vertex the color white
    EmitVertex();

    EndPrimitive();
}

void main() {
    //buildHouse(gl_in[0].gl_Position);
}
/**/

/**
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
} gs_in[];

uniform float time;

out vec2 fTexCoords;

vec3 getNormal() {
    //calculate normal based on the vertex coordinates
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);

    return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal) {
    float magnitude = 2.0f;
    vec3 direction = normal * ((sin(time) + 1.0f) / 2.0f) * magnitude;
    return position + vec4(direction, 0.0f);
}

void main() {
    //buildHouse(gl_in[0].gl_Position);

    vec3 normal = getNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    fTexCoords = gs_in[0].texCoords;
    EmitVertex();

    gl_Position = explode(gl_in[1].gl_Position, normal);
    fTexCoords = gs_in[1].texCoords;
    EmitVertex();

    gl_Position = explode(gl_in[2].gl_Position, normal);
    fTexCoords = gs_in[2].texCoords;
    EmitVertex();

    EndPrimitive();
}
/**/

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.4f;

void generateLine(int index) {
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();

    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0f) * MAGNITUDE;
    EmitVertex();

    EndPrimitive();
}

void main() {
    //draw lines for normals on each vertex
    generateLine(0);
    generateLine(1);
    generateLine(2);
}