#version 450 core

const int height_indices[6] = {2, 0, 1, 2, 3, 1};

layout (location = 0) in vec2 vertex;

uniform int width;
uniform int length;
uniform float space;
uniform vec4 quad;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform samplerBuffer heights;

out float out_height;

float get_height(const int vertex) {
	const int index = gl_InstanceID + (gl_InstanceID / width);

	switch(height_indices[vertex]) {
		case 0 :
			return texelFetch(heights, index).r;
			break;
		case 1 :
			return texelFetch(heights, index + 1).r;
			break;
		case 2 :
			return texelFetch(heights, index + (width + 1)).r;
			break;
		case 3 :
			return texelFetch(heights, index + 1 + (width + 1)).r;
			break;
	}
}

void main() {
	float height = get_height(gl_VertexID);
	out_height = height;
	vec2 position = vec2(gl_InstanceID % width, gl_InstanceID / width);
	float x = (vertex.x + position.x) * space + quad.x;
	float z = (vertex.y + position.y) * space + quad.y;
	gl_Position = projection * view * model * vec4(x, height, z, 1.0);
}