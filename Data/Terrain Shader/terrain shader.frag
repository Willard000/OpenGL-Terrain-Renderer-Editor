#version 450 core

layout (location = 0) out vec4 f_color;

in float out_height;

void main() {
	f_color = vec4(1, out_height / 50.0f, out_height, 1);
}