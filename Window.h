#ifndef WINDOW_H
#define WINDOW_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string_view>

class Window {
public:
	Window();
	~Window();

	void show() const;
	void hide() const;

	void size(int* width, int* height) const;
	void resize(int width, int height) const;

	void set_title(std::string_view title) const;

	GLFWwindow* get();
private:
	GLFWwindow* _window;
};

#endif