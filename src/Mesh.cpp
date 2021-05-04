#include "Mesh.h"

#include "Transform.h"

#include <GL/gl3w.h>

Mesh::Mesh() :
	_vao			( 0 ),
	_vertex_buffer	( 0 ),
	_uv_buffer		( 0 ),
	_normal_buffer	( 0 ),
	_indices_buffer	( 0 )
{}

Mesh::Mesh(std::vector<Texture>&&	    textures,
	std::vector<glm::vec3>&&		    vertices,
	std::vector<glm::vec2>&&		    uvs,
	std::vector<glm::vec3>&&		    normals,
	std::vector<unsigned short>&&       indices ) :
	_textures							( std::move(textures) ),
	_vertices							( std::move(vertices) ),
	_uvs								( std::move(uvs) ),
	_normals							( std::move(normals) ),
	_indices							( std::move(indices) )
{}

Mesh::Mesh(Mesh&& rhs) noexcept :
	_textures				( std::move(rhs._textures) ),
	_vertices				( std::move(rhs._vertices) ),
	_uvs					( std::move(rhs._uvs) ),
	_normals				( std::move(rhs._normals) ),
	_indices				( std::move(rhs._indices) ),
	_vao					( rhs._vao ),
	_vertex_buffer			( rhs._vertex_buffer ),
	_uv_buffer				( rhs._uv_buffer ),
	_normal_buffer			( rhs._normal_buffer ),
	_indices_buffer			( rhs._indices_buffer )
{}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vertex_buffer);
	glDeleteBuffers(1, &_uv_buffer);
	glDeleteBuffers(1, &_normal_buffer);
	glDeleteBuffers(1, &_indices_buffer);
	
	for(auto& texture : _textures) {
		
	}
}

void Mesh::init_buffers() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glCreateBuffers(1, &_vertex_buffer);
	if (!_vertices.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
		glNamedBufferStorage(_vertex_buffer, sizeof(glm::vec3) * _vertices.size(), &_vertices[0], 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	glCreateBuffers(1, &_uv_buffer);
	if (!_uvs.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, _uv_buffer);
		glNamedBufferStorage(_uv_buffer, sizeof(glm::vec2) * _uvs.size(), &_uvs[0], 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}

	glCreateBuffers(1, &_normal_buffer);
	if (!_normals.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, _normal_buffer);
		glNamedBufferStorage(_normal_buffer, sizeof(glm::vec3) * _normals.size(), &_normals[0], 0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	}
	
	if (!_indices.empty()) {
		glCreateBuffers(1, &_indices_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indices_buffer);
		glNamedBufferStorage(_indices_buffer, sizeof(unsigned short) * _indices.size(), &_indices[0], 0);
	}
}

void Mesh::draw(GLuint program, const glm::mat4x4 model, int mode) {
	glBindVertexArray(_vao);
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);

	for (unsigned int i = 0; i < _textures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, _textures[i]._id);
	}

	glDrawElements(mode, _indices.size(), GL_UNSIGNED_SHORT, (void*)0);;
}