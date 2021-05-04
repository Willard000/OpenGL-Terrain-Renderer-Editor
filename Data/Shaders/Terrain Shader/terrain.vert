#version 450 core

const int vertex_indices[6] = {2, 0, 1, 2, 3, 1};

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 uv;

uniform int width;
uniform int length;
uniform float space;
uniform vec4 quad;

uniform vec3 test_light_position;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

layout (binding = 0) uniform samplerBuffer heights;
layout (binding = 1) uniform samplerBuffer normals;

out VS {
	float height;
	vec2 uv;
	vec3 normal;

	vec3 position;
	vec2 position_alpha;
} dest;

float get_height(const int vertex) {
	const int index = gl_InstanceID + (gl_InstanceID / width);

	switch(vertex_indices[vertex]) {
		case 0 :	return texelFetch(heights, index).r;break;
		case 1 :	return texelFetch(heights, index + 1).r;	break;
		case 2 :	return texelFetch(heights, index + (width + 1)).r;	break;
		case 3 :	return texelFetch(heights, index + 1 + (width + 1)).r;	break;
	}
}

vec3 get_normal(const int vertex) {
	const int index = gl_InstanceID + (gl_InstanceID / width);

	switch(vertex_indices[vertex]) {
		case 0 :	return texelFetch(normals, index).xyz;	break;
		case 1 :	return texelFetch(normals, index + 1).xyz;	break;
		case 2 :	return texelFetch(normals, index + (width + 1)).xyz;	break;
		case 3 :	return texelFetch(normals, index + 1 + (width + 1)).xyz;	break;
	}
}

void main() {
	const float height = get_height(gl_VertexID);
	const vec3 normal = get_normal(gl_VertexID);
	const vec2 position = vec2(gl_InstanceID % width, gl_InstanceID / width);
	const float x = (vertex.x + position.x) * space + quad.x;
	const float z = (vertex.y + position.y) * space + quad.y;

	gl_Position = projection * view * model * vec4(x, height, z, 1.0);

	dest.height = height;
	dest.uv = uv;
	dest.normal = normal;

	dest.position = vec3(x, height, z);
	dest.position_alpha = vec2(x / width, z / length);
}