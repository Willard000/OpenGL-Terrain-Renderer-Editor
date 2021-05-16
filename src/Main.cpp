#include <GL/gl3w.h>
#include <GLFW/glfw3.h> 

#include <cassert>

#include "Window.h"
#include "Camera.h"
#include "Clock.h"
#include "Scene.h"
#include "Program.h"

#include "Terrain.h"

#include "Core.h"
#include "Game.h"
#include "Editor.h"
#include "StateManager.h"
#include "ShaderManager.h"

#include <iostream>

int main() {

	HWND console_window = GetConsoleWindow();
	SetWindowPos(console_window, 0, -800, 300, 800, 500, 0);

	if(!glfwInit()) {
		assert(0);
	}

	//const GLfloat black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	Core core;
	core._clock = std::make_unique<Clock>();
	core._window = std::make_unique<Window>();
	Camera::Settings camera_settings;
	core._camera = std::make_unique<Camera>(core._window.get(), camera_settings);
	core._state_manager = std::make_unique<StateManager>();
	core._shader_manager = std::make_unique<ShaderManager>(core._camera.get());

	while (1) {
		glfwPollEvents();
		if(glfwGetKey(core._window->get(), GLFW_KEY_1)) {
			core._state_manager->add(std::make_unique<Game>(&core));
			break;
		}
		if(glfwGetKey(core._window->get(), GLFW_KEY_2)) {
			core._state_manager->add(std::make_unique<Editor>(&core));
			break;
		}
	}

	core._state_manager->run();

	return 0;
}