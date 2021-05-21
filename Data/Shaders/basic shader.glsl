#Vertex
#version 450 core

layout (location = 0) in vec3 vertex;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
	gl_Position = projection * view * model * vec4(vertex.xyz, 1.0);
}
#End
#Fragment
#version 450 core

layout (location = 0) out vec4 f_color;

void main() {
	f_color = vec4(1, 0.1, 1, 1);
}
#End