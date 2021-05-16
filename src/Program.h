#ifndef PROGRAM_H
#define PROGRAM_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string>

/********************************************************************************************************************************************************/

struct ShaderInfo {
	unsigned short		 _type;
	std::string			 _file_path;
};

/********************************************************************************************************************************************************/

struct ProgramFile {
	ProgramFile(const char* file_path);

	std::string _dir;
	std::string _name;

	std::string _vertex_shader;
	std::string _fragment_shader;
	std::string _geometry_shader;
	std::string _compute_shader;
};

/********************************************************************************************************************************************************/

struct Program {
	Program();
	Program(int key, std::string_view file_path);
	
	~Program();

	void load(std::string_view file_path);
	unsigned int load_shaders(const ShaderInfo* program) const;
	
	int				_key;
	std::string		_name;
	GLuint			_id;
};

/********************************************************************************************************************************************************/

#endif