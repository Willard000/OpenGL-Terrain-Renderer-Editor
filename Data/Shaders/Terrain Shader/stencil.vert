#version 450 core

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 position;

out VS {
	vec3 vertex;
} dest;

void main() {
	dest.vertex = vec3(position.x, position.y, position.z);
	gl_Position = projection * view * model * vec4(position.x, 0, position.z, 1.0);
}