#version 450 core

layout (location = 0) out vec4 f_color;

uniform int width;

in float out_height;

void main() {
	f_color = vec4(.1, out_height / width, out_height / width, 1);
}