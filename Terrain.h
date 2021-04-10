#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/gl3w.h>

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>

#include "Program.h"

class TerrainMesh {
public:
	TerrainMesh(Program* program);

protected:
	void create_buffers();
	void draw(glm::vec4 quad, int width, int length, float space, std::vector<GLfloat>& sub_heights);
	
	GLuint _vao, _vertex_buffer, _height_buffer, _height_texture;

	Program* _program;
};

class Terrain : public TerrainMesh {
public:
	Terrain(int width, int length, float space, Program* program);

	void draw(glm::vec3 camera_position, int detail);

	void adjust_tile_height(int index, float height);

	void generate_sub_heights(std::vector<GLfloat>* in_heights, std::vector<GLfloat>* out_heights, int start_index, int width);
private:
	void create_height_buffer();
	void draw(glm::vec2 position, glm::vec4 quad, float space, std::vector<GLfloat>& sub_heights, int start_index, int detail, int depth);

	int _width, _length;
	std::vector<GLfloat> _heights;
	GLuint _height_map;

	float _space;
};

#endif