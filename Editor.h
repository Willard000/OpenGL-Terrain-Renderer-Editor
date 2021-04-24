#ifndef EDITOR_H
#define EDITOR_H

#include "State.h"
#include "Core.h"

#include "Terrain.h"

#include <memory>

class GLFWwindow;

class Editor : public State {
public:
	Editor(Core* core);

	bool handle_input();
	void update();
	void draw();
private:
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	glm::vec3 mouse_world_space();

	Core* _core;

	std::unique_ptr<Terrain> _terrain;
};

#endif