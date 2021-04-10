#ifndef SCENE_H
#define SCENE_H

#include <GL/gl3w.h>

#include "Mesh.h"
#include "Transform.h"
#include "Program.h"

#include <vector>
#include <memory>
#include <string_view>

struct aiNode;
struct aiScene;

class Scene {
public:
	Scene();

	void draw(int mode = GL_TRIANGLES, glm::mat4 transform = glm::mat4(1));

	Scene* new_child();
	void add_child(std::unique_ptr<Scene> child);

	void attach_program(Program* program);
	void set_transform(Transform transform);

	bool load_assimp(std::string_view directory, std::string_view file);
private:
	void attach_program(Program* program, Scene* child) const;
	void construct_scene_from_assimp(const aiScene* ai_scene, aiNode* node, Scene* scene, std::string_view directory, std::string depth) const;

	Scene* _parent;
	std::vector<std::unique_ptr<Scene>> _children;

	std::vector<Mesh> _meshes;
	Transform _transform;
	Program* _program;
};

#endif