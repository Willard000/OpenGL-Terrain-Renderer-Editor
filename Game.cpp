#include "Game.h"

#include <cassert>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "Window.h"
#include "camera.h"
#include "Clock.h"
#include "ShaderManager.h"

#include <iostream>

constexpr GLfloat CLEAR_COLOR[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

Game::Game(Core* core) :
	_core		( core )
{
	assert(_core);

	const auto terrain_shader = _core->_shader_manager->get_program(1);
	const auto stencil_shader = _core->_shader_manager->get_program(2);
	//_terrain = std::make_unique<Terrain>(10, 10, terrain_shader, stencil_shader);
}

bool Game::handle_input() {
	glfwPollEvents();

	double xpos, ypos;
	glfwGetCursorPos(_core->_window->get(), &xpos, &ypos);
	_core->_camera->move_angle((float)xpos, (float)ypos);

	if (glfwGetKey(_core->_window->get(), GLFW_KEY_W)) {
		_core->_camera->move(CAMERA_FORWARD, (float)_core->_clock->get_time());
	}
	if (glfwGetKey(_core->_window->get(), GLFW_KEY_S)) {
		_core->_camera->move(CAMERA_BACKWARD, (float)_core->_clock->get_time());
	}
	if (glfwGetKey(_core->_window->get(), GLFW_KEY_A)) {
		_core->_camera->move(CAMERA_LEFT, (float)_core->_clock->get_time());
	}
	if (glfwGetKey(_core->_window->get(), GLFW_KEY_D)) {
		_core->_camera->move(CAMERA_RIGHT, (float)_core->_clock->get_time());
	}
	if (glfwGetKey(_core->_window->get(), GLFW_KEY_Q)) {
		_core->_camera->move(CAMERA_DOWN, (float)_core->_clock->get_time());
	}
	if (glfwGetKey(_core->_window->get(), GLFW_KEY_E)) {
		_core->_camera->move(CAMERA_UP, (float)_core->_clock->get_time());
	}

	return glfwWindowShouldClose(_core->_window->get()) || glfwGetKey(_core->_window->get(), GLFW_KEY_ESCAPE);
}

void Game::update() {
	_core->_clock->update();
	_core->_window->set_title(std::to_string(_core->_clock->get_fms()));

	_core->_camera->update();
	glUseProgram(2);
	glUniformMatrix4fv(glGetUniformLocation(2, "view"), 1, GL_FALSE, &_core->_camera->get_view()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(2, "projection"), 1, GL_FALSE, &_core->_camera->get_projection()[0][0]);

	//_terrain->update(_core->_camera->get_position(), 3);
}

void Game::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, CLEAR_COLOR);

	//_terrain->draw(_core->_camera->get_position(), 3);

	glfwSwapBuffers(_core->_window->get());

	int r = 0;
	do {
		r = glGetError();
		if (r != 0)
			std::cout << "GLError -> " << r << '\n';
	} while (r != 0);
}