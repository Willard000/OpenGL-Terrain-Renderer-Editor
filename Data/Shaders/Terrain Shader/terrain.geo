#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS {
	float height;
	vec2 uv;
	vec2 vertex;
} source[];

out FS {
	float height;
	vec2 uv;
	vec3 normal;
} dest;

void main() {

	vec3 ab = vec3(source[1].vertex.x, source[1].height, source[1].vertex.y) - 
			  vec3(source[0].vertex.x, source[0].height, source[0].vertex.y);

	vec3 ac = vec3(source[2].vertex.x, source[2].height, source[2].vertex.y) -
			  vec3(source[0].vertex.x, source[0].height, source[0].vertex.y);

	vec3 normal = normalize(cross(ab, ac));

	for(int i = 0; i < gl_in.length(); ++i) {
		gl_Position = gl_in[i].gl_Position;
		
		dest.height = source[i].height;
		dest.uv = source[i].uv;
		dest.normal = normal;
		EmitVertex();
	}

	EndPrimitive();
}