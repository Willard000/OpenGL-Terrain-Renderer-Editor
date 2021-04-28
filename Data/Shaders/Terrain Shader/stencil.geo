#version 450 core

layout (points) in;
layout (line_strip, max_vertices = 64) out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform int width;
uniform int length;

layout (binding = 0) uniform samplerBuffer heights;

const float PI = 3.1415926;

in VS {
    vec3 vertex;
} source[];

float get_height_upper(vec2 coord, int index) {
    coord.x = coord.x - floor(coord.x);
    coord.y = coord.y - floor(coord.y);
    
    vec3 a = vec3(0, texelFetch(heights, index + width + 1).r, 1);
    vec3 b = vec3(0, texelFetch(heights, index).r, 0);
    vec3 c = vec3(1, texelFetch(heights, index + 1).r, 0);

    vec3 n = cross(a-c, b-c);

    float h = a.y - ((coord.x - a.x) * n.x + (coord.y - a.z) * n.z) / n.y;

    return h;
}

float get_height_lower(vec2 coord, int index) {
    coord.x = coord.x - floor(coord.x);
    coord.y = coord.y - floor(coord.y);
    
    vec3 a = vec3(0, texelFetch(heights, index + width + 1).r, 1);
    vec3 b = vec3(1, texelFetch(heights, index + width + 2).r, 1);
    vec3 c = vec3(1, texelFetch(heights, index + 1).r, 0);

    vec3 n = cross(a-c, b-c);

    float h = a.y - ((coord.x - a.x) * n.x + (coord.y - a.z) * n.z) / n.y;

    return h;
}

void main() {

	for (int i = 0; i <= 64; i++) {
       // Angle between each side in radians
       float ang = PI * 2.0 / 63.0 * i;

        // Offset from center of point (0.3 to accomodate for aspect ratio)

        float x = cos(ang) + source[0].vertex.x;
        float y = source[0].vertex.y;
        float z = -sin(ang) + source[0].vertex.z;

        float dx = x - floor(x);
        float dz = z - floor(z);

        float h = 0.0f;
        int index = int(floor(x + int(z) * (width + 1)));
        if(dx + dz > 1.0f) {
            // lower tri
            h = get_height_lower(vec2(x, z), index);
        }
        else {
            // uppper tri
            h = get_height_upper(vec2(x, z), index);
        }

        y = h + 0.01f;

        vec4 offset = vec4(x, y, z, 1.0);
        gl_Position = projection * view * model * offset;
        EmitVertex();
    }

    EndPrimitive();
}