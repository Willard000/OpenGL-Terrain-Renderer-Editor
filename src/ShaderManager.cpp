#include "ShaderManager.h"

#include "FileReader.h"

#include <iostream>
#include <cassert>

ShaderManager::ShaderManager(Camera* camera) :
	_camera				( camera )		
{
	FileReader file("Data\\Shaders\\shaders.txt", FileReader::int_val);

	if(!file.is_read()) {
		assert(NULL);
	}

	for(auto it = file.begin(); it != file.end(); ++it) {
		for(auto itt = it->table.begin(); itt != it->table.end(); ++itt) {
			std::cout << "Load Program -> " << itt->value << '\n';
			
			if(_programs.count(itt->key_val)) {
				std::cout << "Duplicate Program Key -> " << itt->key_val << '\n';
				continue;
			}

			_programs[itt->key_val] = std::make_unique<Program>(itt->key_val, itt->value);
			_camera->attach_program(_programs[itt->key_val].get());
		}
	}
}

Program* ShaderManager::get_program(int key) {
	if (!_programs.count(key)) {
		std::cout << "invalid program key \n";
	}

	return _programs.at(key).get();
}