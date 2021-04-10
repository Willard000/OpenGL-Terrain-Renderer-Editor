#ifndef DEBUG_RECT_H
#define DEBUG_RECT_H

#include <GL/gl3w.h>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

struct RectDesc {
	float width = 0.0f, height = 0.0f, length = 0.0f;
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec4 color = glm::vec4(0, 0, 0, 1);
};

/********************************************************************************************************************************************************/

class RendererRectangle {
public:
	RendererRectangle();
	~RendererRectangle();

	void create_vao();
	void draw_rect(RectDesc rect);
private:
	GLuint _vao;
	GLuint _vertex_buffer;
	GLuint _program;
};

/********************************************************************************************************************************************************/

class Renderer : public RendererRectangle {
public:
	Renderer();
	~Renderer();

	void debug_draw();
	void debug_clear();
	void debug_add_rect(RectDesc rect);

private:
	std::vector<RectDesc> _debug_rects;
};

/********************************************************************************************************************************************************/

#endif