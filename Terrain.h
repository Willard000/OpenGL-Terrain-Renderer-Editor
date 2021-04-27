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
typedef std::vector<GLfloat>						TerrainHeights;
typedef std::vector<glm::vec3>						TerrainNormals;
typedef std::vector<std::array<glm::vec3, 2>>		TerrainFaceNormals;

struct TerrainTile {
	struct Vertex {
		float	  height;
		glm::vec3 normal;
	};

	struct Height {
		float _v0, _v1, _v2, _v3;
	};

	TerrainTile(Vertex v0, Vertex v1, Vertex v2, Vertex v3) :
		_v0 (v0), _v1 (v1), _v2 (v2), _v3 (v3){}

	Vertex _v0, _v1, _v2, _v3;
};

struct TerrainNode {
public:
	TerrainNode(Terrain* root, TerrainNode* parent, float space, glm::vec4 quad);

	void subdivide(glm::vec2 position, int depth = 0);
	void subdivide(int depth = 0);
	void draw(glm::vec2 position, int depth = 0);
	void draw(int depth = 0);
	void create_children();
	void generate_heights(int index);
	void generate_normals();

	bool within_range(glm::vec2 p);
	bool has_children();

	TerrainTile get_tile(size_t index) const;
	TerrainTile::Height get_tile_height(size_t index) const;
	

	Terrain*								_root;
	TerrainNode*							_parent;
	TerrainChildren							_children;

	float									_space;
	glm::vec4								_quad;
	TerrainHeights							_heights;
	TerrainNormals							_normals;
};

class Terrain : public TerrainMesh, public StencilMesh {
public:
	Terrain(int width, int length, int depth, Program* terrain_program, Program* stencil_program);

	void draw(glm::vec3 camera_position);
	void draw_stencil(glm::vec3 position);
	void update(glm::vec3 camera_position);

	void adjust_height(size_t x, size_t y, size_t vertex, float val);
	inline void adjust_height(size_t index, size_t vertex, float val);

	Transform& get_transform();
private:
	void create_height_buffer();
	void create_normal_buffer();

	inline void set_height(size_t x, size_t y, size_t vertex, float val);

	int								_width;
	int								_length;
	int								_depth;
	std::array<int, 4>				_sub_indices;
	GLuint							_height_map;

	TerrainNode						_node;
	Transform						_transform;

	friend struct TerrainNode;
	friend class TerrainMesh;
	friend class StencilMesh;
};

#endif