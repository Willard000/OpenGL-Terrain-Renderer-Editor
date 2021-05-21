#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <fstream>

#include <string>

/********************************************************************************************************************************************************/

struct Program {
	Program();
	Program(int key, std::string_view file_path);
	
	~Program();

	bool load(std::string_view file_path);
	bool load_shader(int type, const char* shader);
	
	int				_key;
	std::string		_name;
	GLuint			_id;
};

/********************************************************************************************************************************************************/

#endif