#include "Terrain.h"

#include <GL/gl3w.h>

#include <cstdint>

#include <SOIL/SOIL2.h>
#include <iostream>

#include "PerlinNoise.hpp"

//-----------------------------------------------------------STENCIL MESH---------------------------------------------------------------------------------------------------------

StencilMesh::StencilMesh(Program* program, Terrain* root) :
	_program		( program ),
	_root			( root )
{
	create_buffers();
}

void StencilMesh::create_buffers() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glUseProgram(_program->_id);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, _root->_height_texture);
}

void StencilMesh::draw(glm::vec3 position) {
	glBindVertexArray(_vao);
	glUseProgram(_program->_id);

	glUniformMatrix4fv(glGetUniformLocation(_program->_id, "model"), 1, GL_FALSE, &_root->_transform.get_model()[0][0]);
	glUniform1i(glGetUniformLocation(_program->_id, "width"), _root->_width);
	glUniform1i(glGetUniformLocation(_program->_id, "length"), _root->_length);
	glUniform3fv(glGetUniformLocation(_program->_id, "position"), 1, &_position[0]);

	glBindBuffer(GL_ARRAY_BUFFER, _root->_height_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * _root->_node._heights.size(), &_root->_node._heights[0]);

	glDrawArrays(GL_POINTS, 0, 1);
}

void StencilMesh::update(glm::vec3 mouse_vector, glm::vec3 offset) {
	const float y = abs((offset.y - 0.0f) / mouse_vector.y);
	const float x = (y * mouse_vector.x + offset.x) / _root->_transform.get_scale().x;
	const float z = (y * mouse_vector.z + offset.z) / _root->_transform.get_scale().z;

	_position = glm::vec3(x, 0.01, z);
}

//-----------------------------------------------------------TERRAIN MESH---------------------------------------------------------------------------------------------------------

constexpr size_t TILE_VERTICES_SIZE = 12;

constexpr GLfloat TILE_VERTICES[] = {
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f
};

TerrainMesh::TerrainMesh(Program* program) :
	_program		( program )
{
	create_buffers();
	create_tile_texture();
}

void TerrainMesh::create_buffers() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glUseProgram(_program->_id);

	glCreateBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glNamedBufferStorage(_vertex_buffer, sizeof(GLfloat) * TILE_VERTICES_SIZE, TILE_VERTICES, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glCreateBuffers(1, &_uv_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _uv_buffer);
	glNamedBufferStorage(_uv_buffer, sizeof(GLfloat) * TILE_VERTICES_SIZE, TILE_VERTICES, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void TerrainMesh::create_tile_texture() {
	_tile_texture = SOIL_load_OGL_texture("Data\\tile.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	if(_tile_texture == 0) {
		std::cout << "SOIL FAILED TO LOAD TILE TEXTURE" << '\n';
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _tile_texture);
}

void TerrainMesh::draw(TerrainNode* node) {
	glBindVertexArray(_vao);
	glUseProgram(_program->_id);

	glUniform1i(glGetUniformLocation(_program->_id, "width"), node->_root->_width);
	glUniform1i(glGetUniformLocation(_program->_id, "length"), node->_root->_length);
	glUniform1f(glGetUniformLocation(_program->_id, "space"), node->_space);
	glUniform4f(glGetUniformLocation(_program->_id, "quad"), node->_quad.x, node->_quad.y, node->_quad.z, node->_quad.w);
	glUniformMatrix4fv(glGetUniformLocation(_program->_id, "model"), 1, GL_FALSE, &node->_root->_transform.get_model()[0][0]);

	glUniform3fv(glGetUniformLocation(_program->_id, "test_light_position"), 1, &node->_root->StencilMesh::_position[0]);

	glBindBuffer(GL_ARRAY_BUFFER, _height_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * node->_heights.size(), &node->_heights[0]);

	glBindBuffer(GL_ARRAY_BUFFER, _normal_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * node->_normals.size(), &node->_normals[0]);

	glDrawArraysInstanced(GL_TRIANGLES, 0, TILE_VERTICES_SIZE / 2, node->_root->_width * node->_root->_length);
}

//-----------------------------------------------------------------TERRAIN Node---------------------------------------------------------------------------------------------------------

TerrainNode::TerrainNode(Terrain* root, TerrainNode* parent, float space, glm::vec4 quad) :
	_root					( root ),
	_parent					( parent ),
	_space					( space ),
	_quad					( quad )
{}

void TerrainNode::subdivide(glm::vec2 position, int depth) {
	if(depth == _root->_depth) {
		return;
	}

	if (!has_children()) {
		create_children();
	}

	for (auto& child : _children) {
		if (child->within_range(position)) {
			child->subdivide(position, depth + 1);
		}
	}
}

void TerrainNode::subdivide(int depth) {
	if (depth == _root->_depth) {
		return;
	}

	if (!has_children()) {
		create_children();
	}

	for (auto& child : _children) {
		child->subdivide(depth + 1);
	}
}

void TerrainNode::draw(glm::vec2 position, int depth) {
	if(depth == _root->_depth || !has_children()) {
		_root->TerrainMesh::draw(this);
		return;
	}

	for(auto& child : _children) {
		if (child->within_range(position)) {
			child->draw(position, depth + 1);
		}
		else {
			_root->TerrainMesh::draw(child.get());
		}
	}
}

void TerrainNode::draw(int depth) {
	if (depth == _root->_depth || !has_children()) {
		_root->TerrainMesh::draw(this);
		return;
	}

	for (auto& child : _children) {
		child->draw(depth + 1);
	}
}


//   0------1
//   |	 /  |	
//   |  /   |		
//   2------3

void TerrainNode::create_children() {
	const auto x = _quad.x;
	const auto z = _quad.y;
	const auto w = _quad.z / 2.0f;
	const auto l = _quad.w / 2.0f;

	glm::vec4 quads[4] = { { x,     z,     w, l},
						   { x + w, z,     w, l},
						   { x,     z + l, w, l},
						   { x + w, z + l, w, l}
	};

	for (size_t i = 0; i < 4; ++i) {
		_children[i] = std::make_unique<TerrainNode>(
			_root, this, _space / 2.0f, quads[i]
		);

		_children[i]->generate_heights(_root->_sub_indices[i]);
		_children[i]->generate_normals();
	}
}

TerrainTile TerrainNode::get_tile(size_t index) const {
	if (index >= _heights.size() || _heights.size() != _normals.size()) {
		assert(0);
	}

	TerrainTile::Vertex v0, v1, v2, v3;

	v0.height = _heights[index];
	v1.height = _heights[index + 1];
	v2.height = _heights[index + _root->_width + 1];
	v3.height = _heights[index + _root->_width + 2];

	v0.normal = _normals[index];
	v1.normal = _normals[index + 1];
	v2.normal = _normals[index + _root->_width + 1];
	v3.normal = _normals[index + _root->_width + 2];

	return TerrainTile(v0, v1, v2, v3);
}

TerrainTile::Height TerrainNode::get_tile_height(size_t index) const {
	TerrainTile::Height height;
	index += index / _root->_width;

	height._v0 = _heights[index];
	height._v1 = _heights[index + 1];
	height._v2 = _heights[index + _root->_width + 1];
	height._v3 = _heights[index + _root->_width + 2];
	return height;
}

void TerrainNode::generate_heights(int index) {
	_heights.clear();
	_heights.resize(_parent->_heights.size());

	auto valid = [&](const size_t& i) {
		if(i < 0 || i > _parent->_heights.size() - 1) {
			return false;
		}
		return true;
	};
			
	auto avg = [&](const size_t& i , const size_t& i2) {
		std::vector<size_t> indices = { i, i2};
		int divisor = 0;
		GLfloat value = 0.0f;
		for (const auto& i : indices) {
			if (valid(i)) {
				value += _parent->_heights.at(i);
				++divisor;
			}
		}
			
		return value / (GLfloat)divisor;
	};
			
	auto avg2 = [&](const int& i, const int& i2, const int& i3, const int& i4) {
		std::vector<int> indices = {i, i2, i3, i4};
		int divisor = 0;
		GLfloat value = 0.0f;
		for(const auto& i : indices) {
			if (valid(i)) {
				value += _parent->_heights.at(i);
				++divisor;
			}
		}
			
		return value / (GLfloat)divisor;
	};

	size_t in_i = index;
	size_t out_i = 0;
	int count = 0;
		
	bool even = true;
	while (out_i < (_heights.size())) {
		if (even) {
			_heights.at(out_i) = _parent->_heights.at(in_i);
			++count;
		
			while (count < _root->_width) {
				++out_i;
				++in_i;
				_heights.at(out_i) = avg(in_i - 1, in_i);
				++count;
		
				++out_i;
				_heights.at(out_i) = _parent->_heights.at(in_i);
				++count;
			}
		
			++in_i;
			in_i += _root->_width / 2;
		}
		else {
			_heights.at(out_i) = avg(in_i - _root->_width - 1, in_i);
			++count;
		
			while (count < _root->_width) {
				++out_i;
				++in_i;
				_heights.at(out_i) = avg2(in_i, in_i - _root->_width - 1, in_i - 1, in_i - _root->_width - 2);
				++count;
		
				++out_i;
				_heights.at(out_i) = avg(in_i - _root->_width - 1, in_i);
				++count;
			}
		
			in_i -= _root->_width / 2;
		}
		
		++out_i;
		count = 0;
		even = !even;
	}
}

void TerrainNode::generate_normals() {
	TerrainFaceNormals face_normals(_root->_width * _root->_length);

	const auto tri_normal = [](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
		auto cross = glm::cross(b - a, c - a);

		return cross;
	};

	for (size_t i = 0; i < face_normals.size(); ++i) {
		const auto tile = get_tile_height(i);
		const auto a1 = glm::vec3(0, tile._v2, 1);
		const auto b1 = glm::vec3(0, tile._v0, 0);
		const auto c1 = glm::vec3(1, tile._v1, 0);
		const auto a2 = glm::vec3(0, tile._v2, 1);
		const auto b2 = glm::vec3(1, tile._v3, 1);
		const auto c2 = glm::vec3(1, tile._v1, 0);

		face_normals[i][0] = tri_normal(c1, b1, a1);
		face_normals[i][1] = tri_normal(a2, b2, c2);
	}

	const auto get_face_normal = [&](const int index, int index2) {
		if (index < 0 || index >= face_normals.size()) {
			return glm::vec3(0, 0, 0);
		}
		return face_normals[index][index2];
	};

	const auto smooth_normal = [&](int index) {
		auto normal = glm::vec3(0, 0, 0);

		normal += get_face_normal(index, 0);
		normal += get_face_normal(index - _root->_width, 0);
		normal += get_face_normal(index - _root->_width, 1);
		normal += get_face_normal(index - _root->_width - 1, 1);
		normal += get_face_normal(index - 1, 0);
		normal += get_face_normal(index - 1, 1);

		return normal;
	};

	_normals.resize((_root->_width + 1) * (_root->_length + 1));
	size_t n_index = 0;
	for (size_t i = 0; i < face_normals.size(); ++i) {
		if (i != 0 && (i + 1) % _root->_width == 0) {
			// right edge
			_normals[n_index++] = smooth_normal(i);

			auto normal = glm::vec3(0, 0, 0);
			normal += get_face_normal(i, 0);
			normal += get_face_normal(i, 1);
			normal += get_face_normal(i - _root->_width, 1);

			_normals[n_index++] = normal;
		}
		else if (i % _root->_width == 0) {
			// left edge
			auto normal = glm::vec3(0, 0, 0);
			normal += get_face_normal(i, 0);
			normal += get_face_normal(i - _root->_width, 0);
			normal += get_face_normal(i - _root->_width, 1);

			_normals[n_index++] = normal;
		}
		else {
			_normals[n_index++] = smooth_normal(i);
		}
	}
}

bool TerrainNode::has_children() {
	const bool r = _children[0] != nullptr;
	for(const auto& child : _children) {
		assert((child != nullptr) == r);
	}

	return r;
}

bool TerrainNode::within_range(glm::vec2 p) {
	glm::vec4 q = { _quad.x * _root->_transform.get_scale().x, _quad.y * _root->_transform.get_scale().z,
					_quad.z * _root->_transform.get_scale().x, _quad.w * _root->_transform.get_scale().z
	};

	return !(p.x < q.x - q.z || p.x > q.x + q.z + q.z
		  || p.y < q.y - q.w || p.y > q.y + q.w + q.w);
}

//-----------------------------------------------------------------TERRAIN--------------------------------------------------------------------------------------------------------------

Terrain::Terrain(int width, int length, int depth, Program* terrain_program, Program* stencil_program) :
	TerrainMesh				( terrain_program ),
	StencilMesh				( stencil_program, this ),
	_width					( width ),
	_length					( length ),
	_depth					( depth ),
	_node					( this, nullptr, 1.0f, glm::vec4(0, 0, width, length) ),
	_sub_indices			( {0, _width / 2, (_width * _length) / 2 + (_length / 2), (_width * _length / 2) + (_length / 2) + (_width / 2) } )
{
	assert(width >= 0 && length >= 0);
	assert(float(width) / 2.0 == width / 2);

	_node._heights.resize((_width + 1) * (_length + 1));

	const siv::PerlinNoise perlin(234);
	for (int i = 0; i < (_width + 1); ++i) {
		for (int k = 0; k < (_length + 1); ++k) {
			_node._heights[i * k] = GLfloat(5 * perlin.accumulatedOctaveNoise2D_0_1(i / _width / 1, k / _length / 5, 25));
		}
	}

	/*for(auto& height : _node._heights) {
		height = rand() % 5;
	}*/

	//_node._heights[59] = 5;

	_node.generate_normals();

	create_height_buffer();
	create_normal_buffer();

	_node.subdivide();
}

void Terrain::create_height_buffer() {
	glBindVertexArray(TerrainMesh::_vao);
	glCreateBuffers(1, &_height_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _height_buffer);
	glNamedBufferStorage(_height_buffer, sizeof(GLfloat) * _node._heights.size(), &_node._heights[0], GL_DYNAMIC_STORAGE_BIT);

	glCreateTextures(GL_TEXTURE_BUFFER, 1, &_height_texture);
	glTextureBuffer(_height_texture, GL_R32F, _height_buffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, _height_texture);
}

void Terrain::create_normal_buffer() {
	glBindVertexArray(TerrainMesh::_vao);
	glCreateBuffers(1, &_normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _normal_buffer);
	glNamedBufferStorage(_normal_buffer, sizeof(glm::vec3) * _node._normals.size(), &_node._normals[0], GL_DYNAMIC_STORAGE_BIT);

	glCreateTextures(GL_TEXTURE_BUFFER, 1, &_normal_texture);
	glTextureBuffer(_normal_texture, GL_RGB32F, _normal_buffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, _normal_texture);
}

void Terrain::update(glm::vec3 camera_position) {
	_node.subdivide(glm::vec2(camera_position.x, camera_position.z));
}

void Terrain::draw(glm::vec3 camera_position) {
	_node.draw(glm::vec2(camera_position.x, camera_position.z));
}

Transform& Terrain::get_transform() {
	return _transform;
}

void Terrain::adjust_height(size_t x, size_t y, size_t vertex, float val) {

}

void Terrain::adjust_height(size_t index, size_t vertex, float val) {

}

void Terrain::set_height(size_t x, size_t y, size_t vertex, float val) {

}