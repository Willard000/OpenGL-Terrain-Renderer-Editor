#include "Terrain.h"

#include <GL/gl3w.h>

#include <cstdint>

#include <SOIL/SOIL2.h>
#include <iostream>

#include "PerlinNoise.hpp"

constexpr size_t TILE_VERTICES_SIZE = 12;

constexpr GLfloat TILE_VERTICES[] = {
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f
};

bool in_quad(glm::vec2 obj, glm::vec4 quad) {
	return !(obj.x < quad.x || obj.x > quad.x + quad.z
		|| obj.y < quad.y || obj.y > quad.y + quad.w);
}

TerrainMesh::TerrainMesh(Program* program) :
	_program		( program )
{
	create_buffers();
}

void TerrainMesh::create_buffers() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glCreateBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glNamedBufferStorage(_vertex_buffer, sizeof(GLfloat) * TILE_VERTICES_SIZE, TILE_VERTICES, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void TerrainMesh::draw(glm::vec4 quad, int width, int length, float space, std::vector<GLfloat>& sub_heights) {
	glBindVertexArray(_vao);
	glUseProgram(_program->_id);

	glUniform1i(glGetUniformLocation(_program->_id, "width"), width);
	glUniform1i(glGetUniformLocation(_program->_id, "length"), length);
	glUniform1f(glGetUniformLocation(_program->_id, "space"), space);
	glUniform4f(glGetUniformLocation(_program->_id, "quad"), quad.x, quad.y, quad.z, quad.w);

	glBindBuffer(GL_ARRAY_BUFFER, _height_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * sub_heights.size(), &sub_heights[0]);

	glDrawArraysInstanced(GL_LINE_STRIP, 0, TILE_VERTICES_SIZE / 2, width * length);

}

Terrain::Terrain(int width, int length, float space, Program* program) :
	TerrainMesh		( program ),
	_width			( width ),
	_length			( length ),
	_space			( space )
{
	_heights.resize((_width + 1) * (_length + 1));
	//_heights.resize(width * length * 100);

	/*int i = 0;
	for(auto& height : _heights) {
		if (i % 2 == 0) {
			++i;
			continue;
		};
		++i;
		height = rand() % 5;
	}
	_heights.resize(_width * _length);*/


	const siv::PerlinNoise perlin(10);
	for (int i = 0; i < (_width + 1); ++i) {
		for (int k = 0; k < (_length + 1); ++k) {
			_heights[i * k] = 50 * perlin.accumulatedOctaveNoise2D_0_1(i / _width / 10, k / _length / 10, 10);
		}
	}

	/*for(int i = 0; i < 101; ++i) {
		_heights[i] = 3.0f;
	}*/

	create_height_buffer();
}

void Terrain::create_height_buffer() {
	glBindVertexArray(_vao);
	glCreateBuffers(1, &_height_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _height_buffer);
	glNamedBufferStorage(_height_buffer, sizeof(GLfloat) * _heights.size(), &_heights[0], GL_DYNAMIC_STORAGE_BIT);

	glCreateTextures(GL_TEXTURE_BUFFER, 1, &_height_texture);
	glTextureBuffer(_height_texture, GL_R32F, _height_buffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, _height_texture);

	glUseProgram(_program->_id);
	glUniform1i(glGetUniformLocation(_program->_id, "heights"), 0);
}

void Terrain::draw(glm::vec3 camera_position, int detail) {
	glBindBuffer(GL_ARRAY_BUFFER, _height_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * _heights.size(), &_heights[0]);

	draw(glm::vec2(camera_position.x, camera_position.z), glm::vec4(0, 0, _width * _space, _length * _space), _space, _heights, 0, detail, 0);
	//TerrainMesh::draw(_width, _length, 1.0f, glm::vec2(0, 0));
}

void Terrain::draw(glm::vec2 position, glm::vec4 quad, float space, std::vector<GLfloat>& sub_heights, int start_index, int detail, int depth) {
	if(depth == detail) {
		TerrainMesh::draw(quad, _width, _length, space, sub_heights);
		return;
	}


	std::vector<GLfloat> n_sub_heights[4];
	const auto quad_one = glm::vec4(quad.x, quad.y, quad.z / 2.0f, quad.w / 2.0f);
	const auto start_one = start_index;
	generate_sub_heights(&sub_heights, &n_sub_heights[0], start_one, _width);

	const auto quad_two = glm::vec4(quad.x + quad.z / 2.0f, quad.y, quad.z / 2.0f, quad.w / 2.0f);
	const auto start_two = start_index + (_width / 2);
	generate_sub_heights(&sub_heights, &n_sub_heights[1], start_two, _width);

	const auto quad_three = glm::vec4(quad.x, quad.y + quad.w / 2.0f, quad.z / 2.0f, quad.w / 2.0f);
	const auto start_three = start_index + (_width * _length) / 2 + (_length / 2);
 	generate_sub_heights(&sub_heights, &n_sub_heights[2], start_three, _width);

	const auto quad_four = glm::vec4(quad.x + quad.z / 2.0f, quad.y + quad.w / 2.0f, quad.z / 2.0f, quad.w / 2.0f);
	const auto start_four = start_index + (_width * _length / 2) + (_length / 2) + (_width / 2);
	generate_sub_heights(&sub_heights, &n_sub_heights[3], start_four, _width);

	if (in_quad(position, quad_one)) {
		draw(position, quad_one, space / 2.0f, n_sub_heights[0], start_one, detail, depth + 1);

		//draw_mesh(quad_two, _width, _length, space / 2.0f, n_sub_heights[1]);
		TerrainMesh::draw(quad_two, _width, _length, space / 2.0f, n_sub_heights[1]);

		//draw_mesh(quad_three, _width, _length, space / 2.0f, n_sub_heights[2]);
		TerrainMesh::draw(quad_three, _width, _length, space / 2.0f, n_sub_heights[2]);

		//draw_mesh(quad_four, _width, _length, space / 2.0f, n_sub_heights[3]);
		TerrainMesh::draw(quad_four, _width, _length, space / 2.0f, n_sub_heights[3]);
	}
	else if (in_quad(position, quad_two)) {
		draw(position, quad_two, space / 2.0f, n_sub_heights[1], start_two, detail, depth + 1);

		//draw_mesh(quad_one, _width, _length, space / 2.0f, n_sub_heights[0]);
		TerrainMesh::draw(quad_one, _width, _length, space / 2.0f, n_sub_heights[0]);

		//draw_mesh(quad_three, _width, _length, space / 2.0f, n_sub_heights[2]);
		TerrainMesh::draw(quad_three, _width, _length, space / 2.0f, n_sub_heights[2]);

		//draw_mesh(quad_four, _width, _length, space / 2.0f, n_sub_heights[3]);
		TerrainMesh::draw(quad_four, _width, _length, space / 2.0f, n_sub_heights[3]);
	}
	else if(in_quad(position, quad_three)) {
		draw(position, quad_three, space / 2.0f, n_sub_heights[2], start_three, detail, depth + 1);

		//draw_mesh(quad_one, _width, _length, space / 2.0f, n_sub_heights[0]);
		TerrainMesh::draw(quad_one, _width, _length, space / 2.0f, n_sub_heights[0]);

		//draw_mesh(quad_two, _width, _length, space / 2.0f, n_sub_heights[1]);
		TerrainMesh::draw(quad_two, _width, _length, space / 2.0f, n_sub_heights[1]);

		//draw_mesh(quad_four, _width, _length, space / 2.0f, n_sub_heights[3]);
		TerrainMesh::draw(quad_four, _width, _length, space / 2.0f, n_sub_heights[3]);
	}
	else if (in_quad(position, quad_four)) {
		draw(position, quad_four, space / 2.0f, n_sub_heights[3], start_four, detail, depth + 1);

		//draw_mesh(quad_one, _width, _length, space / 2.0f, n_sub_heights[0]);
		TerrainMesh::draw(quad_one, _width, _length, space / 2.0f, n_sub_heights[0]);
		
		//draw_mesh(quad_two, _width, _length, space / 2.0f, n_sub_heights[1]);
		TerrainMesh::draw(quad_two, _width, _length, space / 2.0f, n_sub_heights[1]);

		//draw_mesh(quad_three, _width, _length, space / 2.0f, n_sub_heights[2]);
		TerrainMesh::draw(quad_three, _width, _length, space / 2.0f, n_sub_heights[2]);
	}
	else {
		//draw_mesh(quad_one, _width, _length, space, sub_heights);
		TerrainMesh::draw(quad, _width, _length, space, sub_heights);
	}

}

void Terrain::adjust_tile_height(int index, float height) {
	if (index < 0 || index > _heights.size()) {
		return;
	}

	_heights[index] = height;

	glBindBuffer(GL_ARRAY_BUFFER, _height_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * index, sizeof(GLfloat), &_heights[index]);
}

void Terrain::generate_sub_heights(std::vector<GLfloat>* in_heights, std::vector<GLfloat>* out_heights, int start_index, int width) {
	out_heights->clear();
	out_heights->resize(in_heights->size());

	auto valid = [&in_heights](const int& i) {
		if(i < 0 || i > in_heights->size() - 1) {
			return false;
		}
		return true;
	};

	auto avg = [&valid, &in_heights](const int& i , const int& i2) {
		std::vector<int> indices = { i, i2};
		int divisor = 0;
		GLfloat value = 0.0f;
		for (const auto& i : indices) {
			if (valid(i)) {
				value += in_heights->at(i);
				++divisor;
			}
		}

		return value / (GLfloat)divisor;
	};

	auto avg2 = [&valid, &in_heights](const int& i, const int& i2, const int& i3, const int& i4) {
		std::vector<int> indices = {i, i2, i3, i4};
		int divisor = 0;
		GLfloat value = 0.0f;
		for(const auto& i : indices) {
			if (valid(i)) {
				value += in_heights->at(i);
				++divisor;
			}
		}

		return value / (GLfloat)divisor;
	};

	int in_i = start_index;
	int out_i = 0;
	int count = 0;

	bool even = true;
	while (out_i < out_heights->size()) {
		if (even) {
			out_heights->at(out_i) = in_heights->at(in_i);
			++count;

			while (count < width) {
				++out_i;
				++in_i;
				out_heights->at(out_i) = avg(in_i - 1, in_i);
				++count;

				++out_i;
				out_heights->at(out_i) = in_heights->at(in_i);
				++count;
			}

			++in_i;
			in_i += width / 2;
		}
		else {
			out_heights->at(out_i) = avg(in_i - width - 1, in_i);
			++count;

			while (count < width) {
				++out_i;
				++in_i;
				//out_heights->at(out_i) = (in_heights->at(in_i) + in_heights->at(in_i - width) + in_heights->at(in_i - 1) + in_heights->at(in_i - 1 - width)) / 4.0f;
				out_heights->at(out_i) = avg2(in_i, in_i - width - 1, in_i - 1, in_i - width - 2);
				++count;

				++out_i;
				out_heights->at(out_i) = avg(in_i - width - 1, in_i);
				++count;
			}

			in_i -= width / 2;
		}

		++out_i;
		count = 0;
		even = !even;
	}


}