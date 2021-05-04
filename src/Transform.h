#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/gtc/matrix_transform.hpp>

class Transform {
public:
	Transform(
		glm::vec3 position = glm::vec3(0, 0, 0),
		glm::vec3 scale = glm::vec3(1, 1, 1),
		glm::vec3 rotation = glm::vec3(0, 0, 0)
	);

	Transform(const Transform& rhs);

	void init_matrices();

	void set_position(glm::vec3 position);
	void set_scale   (glm::vec3 scale);
	void set_rotation(glm::vec3 rotation);

	glm::vec3 get_position();
	glm::vec3 get_scale();
	glm::vec3 get_rotation();
	glm::mat4 get_model();

private:
	glm::mat4 _model;

	glm::mat4 _position_matrix;
	glm::mat4 _scale_matrix;
	glm::mat4 _rotation_matrix;
	glm::mat4 _rotation_matrix_x;
	glm::mat4 _rotation_matrix_y;
	glm::mat4 _rotation_matrix_z;

	glm::vec3 _position;
	glm::vec3 _scale;
	glm::vec3 _rotation;
};

#endif