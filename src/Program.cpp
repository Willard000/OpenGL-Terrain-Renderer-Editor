#include "Program.h"

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include "FileReader.h"

/********************************************************************************************************************************************************/

#define END_KEYWORD "#End"

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

constexpr int conv_shader_type(std::string_view type) {
	if      (type == "#Vertex")   return GL_VERTEX_SHADER;
	else if (type == "#Fragment") return GL_FRAGMENT_SHADER;
	else if (type == "#Geometry") return GL_GEOMETRY_SHADER;
	else if (type == "#Compute")  return GL_COMPUTE_SHADER;
	else                          return -1;
}

constexpr const char* conv_shader_type(int type) {
	if		(type == GL_VERTEX_SHADER)	 return "Vertex Shader";
	else if (type == GL_FRAGMENT_SHADER) return "Fragment Shader";
	else if (type == GL_GEOMETRY_SHADER) return "Geometry Shader";
	else if (type == GL_COMPUTE_SHADER)  return "Compute Shader";
	else								 return "None";
}

bool Program::load(std::string_view file_path) {
	_id   =			glCreateProgram();
	_name =			file_path.substr(file_path.find_last_of("\\") + 1);
	int				type;
	std::string		type_str;
	std::ifstream   file(file_path.data());

	if(!file.is_open()) {
		std::cout << "Could Not Find Program File Path --> " << file_path << "\n";
		return false;
	}

	const auto parse_shader = [&]() {
		int beg =	static_cast<int>(file.tellg());
		int end =	static_cast<int>(file.tellg());
		std::string keyword;
		int lines = 0;

		while(std::getline(file, keyword)) {
			++lines;

			if (keyword == END_KEYWORD) {
				auto size = end - beg;
				std::string buf;
				buf.resize(size - lines);

				file.seekg(beg, std::ios::beg);
				file.read(&buf[0], size - lines);
				load_shader(type, buf.c_str());
				file.ignore(sizeof(END_KEYWORD));
				return true;
			}

			end = static_cast<int>(file.tellg());
		}

		glDeleteProgram(_id);
		std::cout << "Shader parse error no end marker" << "\n";
		return false;
	};

	bool result = false;
	while(std::getline(file, type_str)) {
		type = conv_shader_type(type_str);
		if(type != -1) {
			result = parse_shader();
		}
	}

	if (result) {
		glLinkProgram(_id);
		std::cout << "Program Loaded -> " << _id << '\n';
		return true;
	}

	return false;
}

bool Program::load_shader(int type, const char* shader) {
	GLuint		id = 0;
	GLint		result = 0;
	GLint		info_log_length = 0;
	auto		loaded = false;

	id = glCreateShader(type);
	glShaderSource(id, 1, &shader, NULL);
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_log_length);
	std::cout << "Shader Result -> " << conv_shader_type(type) << " -> " << ((result == GL_TRUE) ? "Good" : "Bad") << '\n';
	if (info_log_length != 0) {
		std::vector<GLchar> info_log(info_log_length);
		glGetShaderInfoLog(id, info_log_length, 0, &info_log[0]);
		std::cout << "Shader Log -> \n" << &info_log[0] << '\n';

		std::cout << "Reload Shaders... ";
		system("PAUSE");
		std::cout << '\n';
		return false;
	}

	glAttachShader(_id, id);
	glDeleteShader(id);

	return true;
}