#ifndef CAMERA_H
#define CAMERA_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#define CAMERA_FORWARD 1
#define CAMERA_BACKWARD 2
#define CAMERA_LEFT 3
#define CAMERA_RIGHT 4
#define CAMERA_UP 5
#define CAMERA_DOWN 6

#define CAMERA_FREE 0
#define CAMERA_LOCKED 1
#define CAMERA_TOGGLE 2

class Window;
struct Program;

class Camera {
public:
	struct Settings {
		float _field_of_view = 90.0f;
		float _aspect = 1.0f;
		float _z_near = .1f;
		float _z_far = 1000.0f;
		float _h_angle = 3.14f;
		float _v_angle = 0.0f;
	};

	Camera(Window *window, Settings s);

	void update();

	void move(int direction, float time, float speed = 0.0f);
	inline void move_free(int direction, float time, float speed = 0.0f);
	inline void move_locked(int direction, float time, float speed = 0.0f);
	void move_angle(float xpos, float ypos);

	void set_mode(int mode);

	void attach_program(Program* program);

	glm::mat4 get_view() const;
	glm::mat4 get_projection() const;
	glm::vec3 get_position() const;

	int get_mode() const;
private:
	int		 _mode;
	Settings _settings;

	glm::vec3 _position;
	glm::vec3 _direction;
	glm::vec3 _right;
	glm::vec3 _up;

	glm::mat4 _projection;
	glm::mat4 _view;

	Window* _window;
	
	std::vector<Program*> _programs;
};

#endif