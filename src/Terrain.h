#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/gl3w.h>

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <array>
#include <memory>

#include "Program.h"
#include "Transform.h"

#define F_RAISE 0
#define F_SET 1
#define F_AVERAGE 2
#define F_SET_CURRENT 3

#define B_TEXTURE0 0
#define B_TEXTURE1 1
#define B_TEXTURE2 2
#define B_TEXTURE3 3

#define BLEND_ADD 0
#define BLEND_CLEAR 1

#define BLEND_MAP_SIZE 1028

class Terrain;
struct TerrainNode;

/********************************************************************************************************************************************************/

class BrushMesh {
public:
	BrushMesh(Program* program, Terrain* root);

	void update(glm::vec3 mouse_vector, glm::vec3 offset);
	void draw(glm::vec3 position);

	std::vector<std::array<int, 2>> tiles_within_radius();

	void paint_blend_map(int texture, float weight, int flag = BLEND_ADD);
	void raise_height(float val, int flag);

	void update_blend_texture();

	void create_buffers();

	glm::vec3						_position;
	float							_radius;
	GLuint							_vao;
	GLuint							_vertex_buffer;
	Program*						_program;
	Terrain*						_root;
};

/********************************************************************************************************************************************************/

class TerrainMesh {
public:
	TerrainMesh(Program* program);

	void create_buffers();
	void create_tile_textures();
	void draw(TerrainNode* node);
	
	GLuint							_vao;
	GLuint							_vertex_buffer;
	GLuint							_uv_buffer;
	GLuint							_normal_buffer;
	GLuint							_height_buffer;

	GLuint							_height_texture;
	GLuint							_normal_texture;
	std::array<GLuint, 4>			_tile_textures;

	Program*					    _program;
};

/********************************************************************************************************************************************************/

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

/********************************************************************************************************************************************************/

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

	std::array<glm::vec3, 2> calc_face_normal(int index) const;
	glm::vec3 get_face_normal(int index, int triangle) const;
	glm::vec3 generate_normal(int index, int edge) const;

	TerrainNode* find_node(float* x, float* z);

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
	TerrainFaceNormals						_face_normals;
};

/********************************************************************************************************************************************************/

typedef std::array<std::array<glm::vec4, BLEND_MAP_SIZE>, BLEND_MAP_SIZE> BlendMap;

class Terrain {
public:
	Terrain(int width, int length, int depth, Program* terrain_program, Program* brush_program);

	void draw(glm::vec3 camera_position);
	void draw_stencil(glm::vec3 position);
	void update(glm::vec3 camera_position);

	float find_height(glm::vec3 position, glm::vec3 offset);
	bool  above_terrain(glm::vec3 position, glm::vec3 offset, float height);
	float exact_height(float x, float z);

	Transform& get_transform();

	void save(std::string file);
	void load(std::string file);

	void create_height_buffer();
	void create_blend_texture();
	void create_normal_buffer();

	void raise_height(int x, int z, float val, int flag);
	void recalc_normals(int x, int z);

	int								_width;
	int								_length;
	int								_depth;
	std::array<int, 4>				_sub_indices;
	GLuint							_height_map;
	GLuint							_blend_buffer;
	GLuint							_blend_texture;
	BlendMap						_blend_map;

	TerrainNode						_node;
	Transform						_transform;

	std::unique_ptr<TerrainMesh>	_mesh;
	std::unique_ptr<BrushMesh>		_brush_mesh;
};

/********************************************************************************************************************************************************/

#endif