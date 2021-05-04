#include "Window.h"

#include <cassert>

Window::Window() :
	_window				( glfwCreateWindow(1600, 900, "...", nullptr, nullptr) )
{
	glfwSetWindowPos(_window, 150, 100);
	glfwMakeContextCurrent(_window);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	gl3wInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

Window::~Window() {
	glfwDestroyWindow(_window);
}

void Window::show() const {
	glfwShowWindow(_window);
}

void Window::hide() const {
	glfwHideWindow(_window);
}

void Window::size(int* width, int* height) const {
	glfwGetWindowSize(_window, width, height);
}

void Window::resize(int width, int height) const {
	glfwSetWindowSize(_window, width, height);
	glViewport(0, 0, width, height);
}

void Window::set_title(std::string_view title) const {
	glfwSetWindowTitle(_window, title.data());
}

GLFWwindow* Window::get() {
	return _window;
}