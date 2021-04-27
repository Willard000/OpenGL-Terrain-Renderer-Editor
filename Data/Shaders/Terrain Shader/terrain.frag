#version 450 core

layout (location = 0) out vec3 f_color;

uniform vec3 test_light_position;

in VS {
	float height;
	vec2 uv;
	vec3 normal;

	vec3 position;
} source;

layout (binding = 2) uniform sampler2D tile_texture;

void main() {
	vec3 color = vec3(1, 0, 0);
	vec3 light_color = vec3(.5, .5, .5);

	vec3 normal = normalize(source.normal);
	vec3 light_position = vec3(0, 20, 0);
	vec3 light_direction = normalize(light_position - source.position);

	float diff = clamp(dot(normal, light_direction), 0, 1);
	vec3 diffuse = diff * light_color;
	vec3 ambient = vec3(.5, .5, .5);

	f_color = (ambient + diffuse) * color;
}