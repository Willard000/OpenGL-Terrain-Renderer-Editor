#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "Program.h"
#include "Camera.h"

#include <map>
#include <memory>

class ShaderManager {
public:
	ShaderManager(Camera* camera);

	Program* get_program(int key);
private:
	std::map<int, std::unique_ptr<Program>> _programs;

	Camera* _camera;
};

#endif