#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/gl3w.h>

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <array>
#include <memory>

#include "Program.h"
#include "Transform.h"

class Terrain;
struct TerrainNode;

class StencilMesh {
public:
	StencilMesh(Program* program, Terrain* root);

	void update(glm::vec3 mouse_vector, glm::vec3 offset);
	void draw(glm::vec3 position);
protected:
	void create_buffers();

	glm::vec3						_position;

	GLuint							_vao;
	GLuint							_vertex_buffer;

	Program*						_program;
	
	Terrain*						_root;
};

class TerrainMesh {
public:
	TerrainMesh(Program* program);

protected:
	void create_buffers();
	void create_tile_texture();
	void draw(TerrainNode* node);
	
	GLuint							_vao;
	GLuint							_vertex_buffer;
	GLuint							_uv_buffer;
	GLuint							_normal_buffer;
	GLuint							_height_buffer;

	GLuint							_height_texture;
	GLuint							_normal_texture;
	GLuint							_tile_texture;

	Program*					    _program;
};

typedef std::array<std::unique_ptr<TerrainNode>, 4> TerrainChildren;

struct TerrainNode {
public:
	TerrainNode(Terrain* root, TerrainNode* parent, float space, glm::vec4 quad);

	void subdivide(glm::vec2 position, int detail, int depth);
	void draw(glm::vec2 position, int detail, int depth);
	void create_children();
	void generate_sub_heights(int index);
	void generate_sub_normals(int index);

	bool within_range(glm::vec2 p);
	bool has_children();

	Terrain*						_root;
	TerrainNode*					_parent;
	TerrainChildren					_children;

	float							_space;
	glm::vec4						_quad;
	std::vector<GLfloat>			_heights;
	std::vector<glm::vec3>			_normals;
};

class Terrain : public TerrainMesh, public StencilMesh {
public:
	Terrain(int width, int length, Program* terrain_program, Program* stencil_program);

	void draw(glm::vec3 camera_position, unsigned int detail);
	void draw_stencil(glm::vec3 position);
	void update(glm::vec3 camera_position, unsigned int detail);

	Transform& get_transform();
private:
	void create_height_buffer();
	void create_normal_buffer();
	void generate_base_normals();

	int								_width;
	int								_length;
	std::array<int, 4>				_sub_indices;
	GLuint							_height_map;

	TerrainNode						_node;
	Transform						_transform;

	friend struct TerrainNode;
	friend class TerrainMesh;
	friend class StencilMesh;
};

#endif