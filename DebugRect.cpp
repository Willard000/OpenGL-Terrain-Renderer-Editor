#include "DebugRect.h"

/********************************************************************************************************************************************************/

constexpr float rect_vertex_data[] = { 0.0f, 0.0f, 0.0f, // Bottom
										 1.0f, 0.0f, 0.0f,
										 0.0f, 0.0f, 1.0f,

										 0.0f, 0.0f, 1.0f,
										 1.0f, 0.0f, 0.0f,
										 1.0f, 0.0f, 1.0f,

										 0.0f, 1.0f, 0.0f, // Top
										 1.0f, 1.0f, 0.0f,
										 0.0f, 1.0f, 1.0f,

										 0.0f, 1.0f, 1.0f,
										 1.0f, 1.0f, 0.0f,
										 1.0f, 1.0f, 1.0f,

										 0.0f, 0.0f, 0.0f, // Sides
										 0.0f, 1.0f, 0.0f,
										 0.0f, 1.0f, 1.0f,

										 0.0f, 1.0f, 1.0f,
										 0.0f, 0.0f, 0.0f,
										 0.0f, 0.0f, 1.0f,

										 1.0f, 0.0f, 0.0f,
										 1.0f, 1.0f, 0.0f,
										 1.0f, 1.0f, 1.0f,

										 1.0f, 1.0f, 1.0f,
										 1.0f, 0.0f, 0.0f,
										 1.0f, 0.0f, 1.0f,

										 0.0f, 0.0f, 0.0f,
										 0.0f, 1.0f, 0.0f,
										 1.0f, 0.0f, 0.0f,

										 1.0f, 0.0f, 0.0f,
										 0.0f, 1.0f, 0.0f,
										 1.0f, 1.0f, 0.0f,

										 0.0f, 0.0f, 1.0f,
										 0.0f, 1.0f, 1.0f,
										 1.0f, 0.0f, 1.0f,

										 1.0f, 0.0f, 1.0f,
										 0.0f, 1.0f, 1.0f,
										 1.0f, 1.0f, 1.0f,


};

RendererRectangle::RendererRectangle() {
	create_vao();
}

RendererRectangle::~RendererRectangle() {
	glDeleteBuffers(1, &_vertex_buffer);
	glDeleteVertexArrays(1, &_vao);
}

void RendererRectangle::create_vao() {
	glCreateVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_program = 1;
	glUseProgram(_program);

	glCreateBuffers(1, &_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
	glNamedBufferStorage(_vertex_buffer, sizeof(float) * 36 * 3, rect_vertex_data, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void RendererRectangle::draw_rect(RectDesc rect) {
	glBindVertexArray(_vao);
	glUseProgram(_program);

	glUniform1f(glGetUniformLocation(_program, "width"), rect.width);
	glUniform1f(glGetUniformLocation(_program, "height"), rect.height);
	glUniform1f(glGetUniformLocation(_program, "length"), rect.length);
	glUniform3fv(glGetUniformLocation(_program, "position"), 1, &rect.position[0]);
	glUniform4fv(glGetUniformLocation(_program, "color"), 1, &rect.color[0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisable(GL_BLEND);
}

/********************************************************************************************************************************************************/

Renderer::Renderer() {

}

Renderer::~Renderer() {

}

void Renderer::debug_add_rect(RectDesc rect) {
	_debug_rects.push_back(rect);
}

void Renderer::debug_clear() {
	_debug_rects.clear();
}

void Renderer::debug_draw() {
	for (const auto& rect : _debug_rects) {
		draw_rect(rect);
	}
}

/********************************************************************************************************************************************************/