#version 450 core

layout (location = 0) out vec3 f_color;

in VS {
	float height;
	vec2 uv;
	vec3 normal;
} source;

layout (binding = 2) uniform sampler2D tile_texture;

void main() {
	//f_color = vec4(dest.height / 5, dest.height / 5, dest.height / 5, 1);
	//vec3 color = texture(tile_texture, dest.uv).xyz;
	vec3 color;
	color.r = .1;
	color.g = source.height / 5;
	color.b = 1;
	f_color = color;


	vec3 light_color = vec3(.5, .5, .5);
	vec3 dir = normalize(vec3(1, 1, 1));
	float dotp = dot(abs(source.normal), dir);
	float cos_theta = clamp(dotp, .1, 1);

	f_color = color * light_color * cos_theta * 5;
}