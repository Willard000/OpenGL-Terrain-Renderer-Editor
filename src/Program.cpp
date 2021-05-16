#include "Program.h"

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "FileReader.h"

/********************************************************************************************************************************************************/

ProgramFile::ProgramFile(const char* file_path) {
	FileReader file(file_path);

	file.set_section("Program");
	file.read(&_dir, "DIR");
	file.read(&_name, "name");
	file.read(&_vertex_shader, "vertex");
	file.read(&_fragment_shader, "fragment");
	file.read(&_geometry_shader, "geometry");
	file.read(&_compute_shader, "compute");
}

/********************************************************************************************************************************************************/

Program::Program() :
	_key		( -1 ),
	_id			( 0 )
{}

Program::Program(int key, std::string_view file_path) :
	_key		( key ),
	_id			( 0 )
{
	load(file_path);
}

Program::~Program() {
	glDeleteProgram(_id);
}

void Program::load(std::string_view file_path) {
	ProgramFile file(file_path.data());
	ShaderInfo program[] = { {GL_VERTEX_SHADER, }, { GL_GEOMETRY_SHADER, }, { GL_FRAGMENT_SHADER, }, { GL_COMPUTE_SHADER, }, { GL_NONE, } };

	auto append_strings = [](std::string& str, const std::string_view str2, const std::string_view str3) {
		str.reserve(str2.size() + str3.size());
		str.append(str2);
		str.append(str3);
	};

	if (!file._vertex_shader.empty())		append_strings(program[0]._file_path, file._dir, file._vertex_shader);
	if (!file._geometry_shader.empty())		append_strings(program[1]._file_path, file._dir, file._geometry_shader);
	if (!file._fragment_shader.empty())		append_strings(program[2]._file_path, file._dir, file._fragment_shader);
	if (!file._compute_shader.empty())		append_strings(program[3]._file_path, file._dir, file._compute_shader);

	_id = load_shaders(program);
	_name = file._name;
}

GLuint Program::load_shaders(const ShaderInfo* program) const {
	GLuint	    program_id = glCreateProgram();
	GLuint		shader_id = 0;
	GLint		result = 0;
	GLint		info_log_length = 0;
	auto		data = "";
	auto		loaded = false;

	std::vector<GLuint> shader_ids;

	for (unsigned int i = 0; program[i]._type != GL_NONE; ++i) {
		std::string			sdata;
		std::stringstream	ssdata;
		std::fstream		file(program[i]._file_path, std::ios::in);

		if (file.is_open()) {
			ssdata << file.rdbuf();
			file.seekg(0, std::ios::end);
			sdata.reserve((size_t)file.tellg());
			sdata = ssdata.str();
			data = sdata.c_str();
			shader_id = glCreateShader(program[i]._type);
			shader_ids.push_back(shader_id);

			glShaderSource(shader_id, 1, &data, NULL);
			glCompileShader(shader_id);
			glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
			glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
			std::cout << "Shader Result -> " << program[i]._file_path << " -> " << ((result == GL_TRUE) ? "Good" : "Bad") << '\n';
			if (info_log_length != 0) {
				std::vector<GLchar> info_log(info_log_length);
				glGetShaderInfoLog(shader_id, info_log_length, 0, &info_log[0]);
				std::cout << "Shader Log -> \n" << &info_log[0] << '\n';

				std::cout << "Reload Shaders... ";
				system("PAUSE");
				std::cout << '\n';
			}
		}
		else {
			std::cout << "Couldn't open program file -> " << program[i]._file_path << '\n';
		}
	}

	for (auto shader_id : shader_ids) {
		glAttachShader(program_id, shader_id);
	}

	glLinkProgram(program_id);

	for (auto shader_id : shader_ids) {
		glDetachShader(program_id, shader_id);
		glDeleteShader(shader_id);
	}

	std::cout << "Program Loaded -> " << program_id << '\n';

	return program_id;
}