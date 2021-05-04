#include "Editor.h"

#include "Window.h"
#include "Clock.h"
#include "Camera.h"
#include "ShaderManager.h"

#include "Terrain.h"

#include <iostream>

constexpr GLfloat CLEAR_COLOR[4] = { 0.0f, 0.2f, 0.4f, 1.0f };

Editor::Editor(Core* core) :
	_core		( core )
{
	const auto terrain_shader = _core->_shader_manager->get_program(1);
	const auto stencil_shader = _core->_shader_manager->get_program(2);
	_terrain = std::make_unique<Terrain>(100, 100, 0, terrain_shader, stencil_shader);
	_terrain->get_transform().set_scale(glm::vec3(10.0f, 10.0f, 10.0f));
	_terrain->load("Data\\terrain.txt");

	glfwSetWindowUserPointer(_core->_window->get(), this);
	glfwSetKeyCallback(_core->_window->get(), &Editor::key_callback);
	glfwSetScrollCallback(_core->_window->get(), &Editor::scroll_callback);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
}

bool Editor::handle_input() {
	glfwPollEvents();

	if(!glfwGetWindowAttrib(_core->_window->get(), GLFW_FOCUSED)) {
		return glfwWindowShouldClose(_core->_window->get()) || glfwGetKey(_core->_window->get(), GLFW_KEY_ESCAPE);
	}

	if (_core->_camera->get_mode() == CAMERA_FREE) {
		double xpos, ypos;
		glfwGetCursorPos(_core->_window->get(), &xpos, &ypos);
		_core->_camera->move_angle((float)xpos, (float)ypos);
	}

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

	if (_edit_mode == EDIT_TERRAIN) {
		if (glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_LEFT)) {
			_terrain->StencilMesh::raise_height(.01f, 0);
		}
		if (glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_RIGHT)) {
			_terrain->StencilMesh::raise_height(-.01f, 0);
		}
		if (glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_MIDDLE)) {
			_terrain->StencilMesh::raise_height(0.0f, 1);
		}
		if (glfwGetKey(_core->_window->get(), GLFW_KEY_R)) {
			_terrain->StencilMesh::raise_height(0.0f, 2);
		}
	}

	if (_edit_mode == EDIT_TEXTURE) {
		if (glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_LEFT)) {
			_terrain->StencilMesh::paint_blend_map(B_TEXTURE0, 0.1f);
		}
		if (glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_RIGHT)) {
			_terrain->StencilMesh::paint_blend_map(B_TEXTURE1, 0.1f);
		}
		if (glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_4)) {
			_terrain->StencilMesh::paint_blend_map(B_TEXTURE2, 0.1f);
		}
		if(glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_5)) {
			_terrain->StencilMesh::paint_blend_map(B_TEXTURE3, 0.1f);
		}
		if (glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_MIDDLE)) {
			_terrain->StencilMesh::paint_blend_map(B_TEXTURE0, 0, BLEND_CLEAR);
		}
	}

	return glfwWindowShouldClose(_core->_window->get()) || glfwGetKey(_core->_window->get(), GLFW_KEY_ESCAPE);
}

void Editor::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	auto editor = static_cast<Editor*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		editor->_core->_camera->set_mode(CAMERA_TOGGLE);
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		editor->set_mode(EDIT_TERRAIN);
	}

	if(key == GLFW_KEY_3 && action == GLFW_PRESS) {
		editor->set_mode(EDIT_TEXTURE);
	}

	if(key == GLFW_KEY_Z && action == GLFW_PRESS) {
		const auto radius = editor->_terrain->StencilMesh::get_radius();
		if (radius > 1) {
			editor->_terrain->StencilMesh::set_radius(radius - 1);
		}
	}
	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		editor->_terrain->StencilMesh::set_radius(editor->_terrain->StencilMesh::get_radius() + 1);
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		editor->_terrain->save("Data\\terrain.txt");
		std::cout << "Terrain Saved \n";
	}
}

void Editor::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
		return;
	}

	auto editor = static_cast<Editor*>(glfwGetWindowUserPointer(window));

	if(yoffset > 0) {
		editor->_terrain->set_radius(editor->_terrain->get_radius() - 0.1f);
	}

	if(yoffset < 0) {
		editor->_terrain->set_radius(editor->_terrain->get_radius() + 0.1f);
	}
}

void Editor::update() {
	_core->_clock->update();
	_core->_window->set_title(std::to_string(_core->_clock->get_fms()));
	_core->_camera->update();

	//_terrain->update(_core->_camera->get_position());
	_terrain->StencilMesh::update(mouse_world_space(), _core->_camera->get_position());
}

void Editor::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, CLEAR_COLOR);

	_terrain->draw(_core->_camera->get_position());
	_terrain->StencilMesh::draw(glm::vec3(0, 0, 0));

	glfwSwapBuffers(_core->_window->get());

	int r = 0;
	do {
		r = glGetError();
		if (r != 0)
			std::cout << "GLError -> " << r << '\n';
	} while (r != 0);
}

glm::vec3 Editor::mouse_world_space() {
	double x, y;
	int width, height;
	glfwGetCursorPos(_core->_window->get(), &x, &y);
	glfwGetWindowSize(_core->_window->get(), &width, &height);
	const float x_norm = static_cast<float>(x) / static_cast<float>(width / 2) - 1.0f;
	const float y_norm = -(static_cast<float>(y) / static_cast<float>(height / 2) - 1.0f);

	//				       inverse projection							    clip space
	glm::vec4 view_space = glm::inverse(_core->_camera->get_projection()) * glm::vec4(x_norm, y_norm, -1.0f, 1.0f);
	view_space.z = -1.0f;
	view_space.w = 0.0f;

	//	normalized world space	 inverse view						 view_space
	return glm::normalize(glm::inverse(_core->_camera->get_view()) * view_space);
}

void Editor::set_mode(int mode) {
	switch(mode) {
	case EDIT_TERRAIN: _edit_mode = EDIT_TERRAIN; break;
	case EDIT_TEXTURE: _edit_mode = EDIT_TEXTURE; break;
	default:									  break;
	}
}