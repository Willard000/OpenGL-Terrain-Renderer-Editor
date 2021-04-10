#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <cassert>

#include "Window.h"
#include "Camera.h"
#include "Clock.h"
#include "Scene.h"
#include "Program.h"

#include "Terrain.h"

#include <iostream>

int main() {

	if(!glfwInit()) {
		assert(0);
	}

	const GLfloat black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	Window window;
	Camera::Settings camera_settings;
	Camera camera(&window, camera_settings);
	Clock clock;

	auto world = std::make_unique<Scene>();
	auto object = world->new_child();

	//object->load_assimp("Data\\Clay Rock\\", "Clay Rock.obj");
	Program program(0, "Data\\Terrain Shader\\Terrain shader.txt");
	//object->attach_program(&program);

	//Transform transform(glm::vec3(0, 0, -5));
	//object->set_transform(transform);

	Terrain terrain(10, 10, 100.0f, &program);

	while(!glfwWindowShouldClose(window.get()) && !glfwGetKey(window.get(), GLFW_KEY_ESCAPE)) {
		clock.update();
		window.set_title(std::to_string(clock.get_fms()));

		camera.update();
		glUseProgram(program._id);
		glUniformMatrix4fv(glGetUniformLocation(program._id, "view"), program._id, GL_FALSE, &camera.get_view()[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program._id, "projection"), program._id, GL_FALSE, &camera.get_projection()[0][0]);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearBufferfv(GL_COLOR, 0, black);

		world->draw(GL_TRIANGLES);

		terrain.draw(camera.get_position(), 5);

		glfwSwapBuffers(window.get());

		int r = 0;
		do {
			r = glGetError();
			if (r != 0)
				std::cout << "GLError -> " << r << '\n';
		} while (r != 0);

		glfwPollEvents();

		double xpos, ypos;
		glfwGetCursorPos(window.get(), &xpos, &ypos);
		camera.move_angle((float)xpos, (float)ypos);

		if(glfwGetKey(window.get(), GLFW_KEY_W)) {
			camera.move(CAMERA_FORWARD, (float)clock.get_time());
		}
		if(glfwGetKey(window.get(), GLFW_KEY_S)) {
			camera.move(CAMERA_BACKWARD, (float)clock.get_time());
		}
		if(glfwGetKey(window.get(), GLFW_KEY_A)) {
			camera.move(CAMERA_LEFT, (float)clock.get_time());
		}
		if(glfwGetKey(window.get(), GLFW_KEY_D)) {
			camera.move(CAMERA_RIGHT, (float)clock.get_time());
		}
		if(glfwGetKey(window.get(), GLFW_KEY_Q)) {
			camera.move(CAMERA_DOWN, (float)clock.get_time());
		}
		if(glfwGetKey(window.get(), GLFW_KEY_E)) {
			camera.move(CAMERA_UP, (float)clock.get_time());
		}
	}


	return 0;
}