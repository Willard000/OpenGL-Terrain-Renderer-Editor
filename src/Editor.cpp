#include "Editor.h"

#include "Window.h"
#include "Clock.h"
#include "Camera.h"
#include "ShaderManager.h"

#include "Terrain.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <iostream>

constexpr GLfloat CLEAR_COLOR[4] = { 0.0f, 0.2f, 0.4f, 1.0f };

/********************************************************************************************************************************************************/

Editor::Editor(Core* core) :
	_core			( core ),
	_brush_window	( this )
{

	TerrainShaders terrain_shaders(
		_core->_shader_manager->get_program(1),
		_core->_shader_manager->get_program(2),
		nullptr
	);

	GLuint vao;
	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	_terrain = std::make_unique<Terrain>(100, 100, 0, vao, terrain_shaders);
	_terrain->get_transform().set_scale(glm::vec3(10.0f, 10.0f, 10.0f));
	_terrain->load("Data\\terrain.txt");

	glfwSetWindowUserPointer(_core->_window->get(), this);
	glfwSetKeyCallback(_core->_window->get(), &Editor::key_callback);
	glfwSetScrollCallback(_core->_window->get(), &Editor::scroll_callback);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glViewport(100, 50, 1400, 800);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	setup_imgui();
}

Editor::~Editor() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Editor::first_frame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	_brush_window.update();
}

bool Editor::handle_input() {
	glfwPollEvents();

	const bool ignore_mouse_input = ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive();

	if(!glfwGetWindowAttrib(_core->_window->get(), GLFW_FOCUSED)) {
		return glfwWindowShouldClose(_core->_window->get()) || glfwGetKey(_core->_window->get(), GLFW_KEY_ESCAPE);
	}

	if (!ignore_mouse_input && _core->_camera->get_mode() == CAMERA_FREE) {
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

	if (!ignore_mouse_input && _brush_window._terrain) {
		if (glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_LEFT)) {
			if(_brush_window._raise)   _terrain->_brush_mesh->raise_height(_brush_window._raise_value, F_RAISE);
			if(_brush_window._set)     _terrain->_brush_mesh->raise_height(0.0f, F_SET_CURRENT);
			if(_brush_window._average) _terrain->_brush_mesh->raise_height(0.0f, F_AVERAGE);
			if(_brush_window._flatten) _terrain->_brush_mesh->raise_height(0.0f, F_SET);
		}
		if (glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_RIGHT)) {
			if (_brush_window._raise) _terrain->_brush_mesh->raise_height(-_brush_window._raise_value, F_RAISE);
		}
	}

	if (!ignore_mouse_input && _brush_window._texture) {
		if (glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_LEFT)) {
			switch (_brush_window._texture_index) {
			case 0:	_terrain->_brush_mesh->paint_blend_map(B_TEXTURE0, 0.4f); break;
			case 1: _terrain->_brush_mesh->paint_blend_map(B_TEXTURE1, 0.4f); break;
			case 2: _terrain->_brush_mesh->paint_blend_map(B_TEXTURE2, 0.4f); break;
			case 3: _terrain->_brush_mesh->paint_blend_map(B_TEXTURE3, 0.4f); break;
			default:													      break;
			}
		}
		if (glfwGetMouseButton(_core->_window->get(), GLFW_MOUSE_BUTTON_MIDDLE)) {
			_terrain->_brush_mesh->paint_blend_map(B_TEXTURE0, 0, BLEND_CLEAR);
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

	if(key == GLFW_KEY_Z && action == GLFW_PRESS) {
		const auto radius = editor->_terrain->_brush_mesh->_radius;
		if (radius > 1) {
			editor->_terrain->_brush_mesh->_radius -= 1.0f;
		}
	}
	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		editor->_terrain->_brush_mesh->_radius += 1.0f;
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
		editor->_terrain->_brush_mesh->_radius -= 0.1f;
	}

	if(yoffset < 0) {
		editor->_terrain->_brush_mesh->_radius += 0.1f;
	}
}

void Editor::update() {
	_core->_clock->update();
	_core->_window->set_title(std::to_string(_core->_clock->get_fms()));

	_core->_camera->update();

	//_terrain->update(_core->_camera->get_position());
	_terrain->_brush_mesh->update(_core->_camera->mouse_to_3d_vector(), _core->_camera->get_position());
}

void Editor::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, CLEAR_COLOR);

	_terrain->draw(_core->_camera->get_position());
	_terrain->_brush_mesh->draw(glm::vec3(0, 0, 0));

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(_core->_window->get());

	int r = 0;
	do {
		r = glGetError();
		if (r != 0)
			std::cout << "GLError -> " << r << '\n';
	} while (r != 0);
}

void Editor::setup_imgui() {
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(_core->_window->get(), false);
	ImGui_ImplOpenGL3_Init("#version 450");
}

/********************************************************************************************************************************************************/

EditorWindow::EditorWindow(Editor* editor) :
	_editor				( editor )
{}

/********************************************************************************************************************************************************/

BrushWindow::BrushWindow(Editor* editor) :
	EditorWindow		( editor ),
	_raise_value		( 0.1f ),
	_terrain			( true ),
	_texture			( false ),
	_raise				( true ),
	_set				( false ),
	_average			( false ),
	_flatten			( false ),
	_texture_index		( 0 )
{}

void BrushWindow::update() {
	ImGui::Begin("Brush");
	ImGui::SliderFloat("Radius", &_editor->_terrain->_brush_mesh->_radius, 1.0f, 100.0f);
	
	if (ImGui::Checkbox("Terrain", &_terrain))		_texture = false;

	if(ImGui::TreeNodeEx("Terrain", ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::SliderFloat("Raise/Lower Value", &_raise_value, 0.0f, 1.0f);

		if(ImGui::Checkbox("Raise/Lower", &_raise))	{ _set   = false; _average = false; _flatten = false; }
		if(ImGui::Checkbox("Set", &_set))			{ _raise = false; _average = false; _flatten = false; }
		if(ImGui::Checkbox("Average", &_average))	{ _raise = false; _set     = false; _flatten = false; }
		if(ImGui::Checkbox("Flatten", &_flatten))   { _raise = false; _set     = false; _average = false; }
		ImGui::TreePop();
	}

	if(ImGui::Checkbox("Texture", &_texture))		_terrain = false;
	if(ImGui::TreeNodeEx("Texture", ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen)) {

		//ImGui::Image((void*)(intptr_t)_editor->_terrain->_mesh->_tile_textures.at(0), ImVec2(64, 64));
		if (ImGui::ImageButton((void*)(intptr_t)_editor->_terrain->_mesh->_tile_textures.at(0), ImVec2(64, 64))) _texture_index = 0;
		ImGui::SameLine();
		if (ImGui::ImageButton((void*)(intptr_t)_editor->_terrain->_mesh->_tile_textures.at(1), ImVec2(64, 64))) _texture_index = 1;
		ImGui::SameLine();
		if (ImGui::ImageButton((void*)(intptr_t)_editor->_terrain->_mesh->_tile_textures.at(2), ImVec2(64, 64))) _texture_index = 2;
		ImGui::SameLine();
		if (ImGui::ImageButton((void*)(intptr_t)_editor->_terrain->_mesh->_tile_textures.at(3), ImVec2(64, 64))) _texture_index = 3;

		ImGui::TreePop();
	}

	ImGui::End();
}

/********************************************************************************************************************************************************/