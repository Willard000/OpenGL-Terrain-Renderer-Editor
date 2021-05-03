#version 450 core

layout (location = 0) out vec3 f_color;

uniform vec3 test_light_position;

in VS {
	float height;
	vec2 uv;
	vec3 normal;

	vec3 position;
	vec2 position_alpha;
} source;

layout (binding = 2) uniform sampler2D tile_texture;
layout (binding = 3) uniform sampler2D blend_map;

void main() {
	//vec3 color = texture(tile_texture, source.uv).xyz;
	//vec3 color = vec3(source.height / 50, .6, .2);
	vec3 color;
	if(abs(normalize(source.normal).y) < .99) {
		color = vec3(source.height/ 100, source.height/ 100, .2);
		color = texture(tile_texture, vec2(source.uv.x + .5, source.uv.y)).xyz;
	}
	else {
		color = vec3(0, .6, source.height / 20);
		color = texture(tile_texture, source.uv).xyz;
	}

	vec2 new_uv = vec2(source.position_alpha.x, source.position_alpha.y);

	color = texture(blend_map, new_uv).rgb;

	vec3 light_color = vec3(.5, .5, .5);

	vec3 normal = normalize(source.normal);
	vec3 light_position = vec3(0, 20, 0);
	vec3 light_direction = normalize(light_position - source.position);

	float diff = clamp(dot(normal, light_direction), 0, 1);
	vec3 diffuse = diff * light_color;
	vec3 ambient = vec3(.5, .5, .5);

	f_color = (ambient + diffuse) * color;
}