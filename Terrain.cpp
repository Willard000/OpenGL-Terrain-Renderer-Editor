#include "Terrain.h"

#include <GL/gl3w.h>

#include <cstdint>

#include <SOIL/SOIL2.h>
#include <iostream>

#include "PerlinNoise.hpp"

//-----------------------------------------------------------TERRAIN MESH---------------------------------------------------------------------------------------------------------


constexpr size_t TILE_VERTICES_SIZE = 12;

constexpr GLfloat TILE_VERTICES[] = {
	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f
};

bool in_quad(glm::vec2 obj, glm::vec4 quad) {
	return !(obj.x < quad.x - quad.z || obj.x > quad.x + quad.z + quad.z 
		|| obj.y < quad.y - quad.w|| obj.y > quad.y + quad.w + quad.w);
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

void TerrainMesh::draw(TerrainNode* node) {
	glBindVertexArray(_vao);
	glUseProgram(_program->_id);

	glUniform1i(glGetUniformLocation(_program->_id, "width"), node->_root->_width);
	glUniform1i(glGetUniformLocation(_program->_id, "length"), node->_root->_length);
	glUniform1f(glGetUniformLocation(_program->_id, "space"), node->_space);
	glUniform4f(glGetUniformLocation(_program->_id, "quad"), node->_quad.x, node->_quad.y, node->_quad.z, node->_quad.w);
	glUniformMatrix4fv(glGetUniformLocation(_program->_id, "model"), 1, GL_FALSE, &node->_root->_transform.get_model()[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, _height_buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * node->_heights.size(), &node->_heights[0]);

	glDrawArraysInstanced(GL_LINE_STRIP, 0, TILE_VERTICES_SIZE / 2, node->_root->_width * node->_root->_length);

}

//-----------------------------------------------------------------TERRAIN Node---------------------------------------------------------------------------------------------------------

TerrainNode::TerrainNode(Terrain* root, TerrainNode* parent, float space, glm::vec4 quad) :
	_root					( root ),
	_parent					( parent ),
	_space					( space ),
	_quad					( quad )
{}

void TerrainNode::subdivide(glm::vec2 position, int detail, int depth) {
	if(depth == detail) {
		return;
	}

	if (!has_children()) {
		create_children();
	}

	for (auto& child : _children) {
		if (child->within_range(position)) {
			child->subdivide(position, detail, depth + 1);
		}
	}
}

void TerrainNode::draw(glm::vec2 position, int detail, int depth) {
	if(depth == detail || !has_children()) {
		_root->TerrainMesh::draw(this);
		return;
	}

	for(auto& child : _children) {
		if (child->within_range(position)) {
			child->draw(position, detail, depth + 1);
		}
		else {
			_root->TerrainMesh::draw(child.get());
		}
	}
}


//   0------1
//   |	    |	
//   |	    |		
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

		_children[i]->generate_sub_heights(_root->_sub_height_indices[i]);
	}
}

void TerrainNode::generate_sub_heights(int index) {
	const auto width = _root->_width;
	const auto p_heights = &_parent->_heights;
	_heights.clear();
	_heights.resize(p_heights->size());
	
	auto valid = [&](const size_t& i) {
		if(i < 0 || i > p_heights->size() - 1) {
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
				value += p_heights->at(i);
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
				value += p_heights->at(i);
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
			_heights.at(out_i) = p_heights->at(in_i);
			++count;
	
			while (count < width) {
				++out_i;
				++in_i;
				_heights.at(out_i) = avg(in_i - 1, in_i);
				++count;
	
				++out_i;
				_heights.at(out_i) = p_heights->at(in_i);
				++count;
			}
	
			++in_i;
			in_i += width / 2;
		}
		else {
			_heights.at(out_i) = avg(in_i - width - 1, in_i);
			++count;
	
			while (count < width) {
				++out_i;
				++in_i;
				_heights.at(out_i) = avg2(in_i, in_i - width - 1, in_i - 1, in_i - width - 2);
				++count;
	
				++out_i;
				_heights.at(out_i) = avg(in_i - width - 1, in_i);
				++count;
			}
	
			in_i -= width / 2;
		}
	
		++out_i;
		count = 0;
		even = !even;
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

Terrain::Terrain(int width, int length, Program* program) :
	TerrainMesh				( program ),
	_width					( width ),
	_length					( length ),
	_node					( this, nullptr, 1.0f, glm::vec4(0, 0, width, length) ),
	_sub_height_indices		( {0, _width / 2, (_width * _length) / 2 + (_length / 2), (_width* _length / 2) + (_length / 2) + (_width / 2) } )
{
	assert(width >= 0 && length >= 0);
	assert(float(width) / 2.0 == width / 2);

	_node._heights.resize((_width + 1) * (_length + 1));

	const siv::PerlinNoise perlin(234);
	for (int i = 0; i < (_width + 1); ++i) {
		for (int k = 0; k < (_length + 1); ++k) {
			_node._heights[i * k] = GLfloat(_width * perlin.accumulatedOctaveNoise2D_0_1(i / _width / 10, k / _length / 10, 10));
		}
	}

	create_height_buffer();
}

void Terrain::create_height_buffer() {
	glBindVertexArray(_vao);
	glCreateBuffers(1, &_height_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _height_buffer);
	glNamedBufferStorage(_height_buffer, sizeof(GLfloat) * _node._heights.size(), &_node._heights[0], GL_DYNAMIC_STORAGE_BIT);

	glCreateTextures(GL_TEXTURE_BUFFER, 1, &_height_texture);
	glTextureBuffer(_height_texture, GL_R32F, _height_buffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, _height_texture);

	glUseProgram(_program->_id);
	glUniform1i(glGetUniformLocation(_program->_id, "heights"), 0);
}

void Terrain::update(glm::vec3 camera_position, unsigned int detail) {
	_node.subdivide(glm::vec2(camera_position.x, camera_position.z), detail, 0U);
}

void Terrain::draw(glm::vec3 camera_position, unsigned int detail) {
	_node.draw(glm::vec2(camera_position.x, camera_position.z), detail, 0U);
}

Transform& Terrain::get_transform() {
	return _transform;
}