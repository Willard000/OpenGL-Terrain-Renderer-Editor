#version 450 core

in vec2 out_uv;

layout (location = 0) out vec4 f_color;

uniform sampler2D texture_sampler;

void main() {
	f_color = texture(texture_sampler, out_uv);
	if(f_color.a < 0.1) {
		discard;
	}
}