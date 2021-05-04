#ifndef EDITOR_H
#define EDITOR_H

#include "State.h"
#include "Core.h"

#include "Terrain.h"

#include <memory>

#define EDIT_TERRAIN 0 
#define EDIT_TEXTURE 1

class GLFWwindow;

class Editor : public State {
public:
	Editor(Core* core);

	bool handle_input();
	void update();
	void draw();

	void set_mode(int mode);
private:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	int			_edit_mode;

	glm::vec3 mouse_world_space();

	Core* _core;

	std::unique_ptr<Terrain> _terrain;
};

#endif