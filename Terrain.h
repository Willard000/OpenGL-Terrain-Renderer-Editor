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

class TerrainMesh {
public:
	TerrainMesh(Program* program);

protected:
	void create_buffers();
	void draw(TerrainNode* node);
	
	GLuint							_vao;
	GLuint							_vertex_buffer;
	GLuint							_height_buffer;
	GLuint							_height_texture;

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

	bool within_range(glm::vec2 p);
	bool has_children();

	Terrain*						_root;
	TerrainNode*					_parent;
	TerrainChildren					_children;

	float							_space;
	glm::vec4						_quad;
	std::vector<GLfloat>			_heights;
};

class Terrain : public TerrainMesh {
public:
	Terrain(int width, int length, Program* program);

	void draw(glm::vec3 camera_position, unsigned int detail);
	void update(glm::vec3 camera_position, unsigned int detail);

	Transform& get_transform();
private:
	void create_height_buffer();

	int								_width;
	int								_length;
	std::array<int, 4>				_sub_height_indices;
	GLuint							_height_map;

	TerrainNode						_node;
	Transform						_transform;

	friend struct TerrainNode;
	friend class TerrainMesh;
};












#endif