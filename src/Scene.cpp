#include "Scene.h"

#include <string_view>

#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <SOIL/SOIL2.h>

#include <iostream>
#include <iomanip>

void copy_mesh(aiMesh* ai_mesh, aiMaterial* ai_material, Mesh* mesh, std::string_view directory) {
	mesh->_vertices.reserve(ai_mesh->mNumVertices);
	mesh->_uvs.reserve(ai_mesh->mNumVertices);
	mesh->_normals.reserve(ai_mesh->mNumVertices);

	for (unsigned int i = 0; i < ai_mesh->mNumVertices; ++i) {
		const aiVector3D vertex = ai_mesh->mVertices[i];
		const aiVector3D normal = ai_mesh->mNormals[i];
		mesh->_vertices.push_back(std::move(glm::vec3(vertex.x, vertex.y, vertex.z)));
		mesh->_normals.push_back(std::move(glm::vec3(normal.x, normal.y, normal.z)));

		if (ai_mesh->HasTextureCoords(0)) {
			const aiVector3D uv = ai_mesh->mTextureCoords[0][i];
			mesh->_uvs.push_back(std::move(glm::vec2(uv.x, uv.y)));
		}
	}

	mesh->_indices.reserve(ai_mesh->mNumFaces);
	for (unsigned int i = 0; i < ai_mesh->mNumFaces; ++i) {
		mesh->_indices.push_back(ai_mesh->mFaces[i].mIndices[0]);
		mesh->_indices.push_back(ai_mesh->mFaces[i].mIndices[1]);
		mesh->_indices.push_back(ai_mesh->mFaces[i].mIndices[2]);
	}

	if (ai_material) {
		auto load_textures = [ai_material, directory, mesh](aiTextureType type, std::string_view type_name) {
			for(unsigned int i = 0; i < ai_material->GetTextureCount(type); ++i) {
				aiString string;
				ai_material->GetTexture(type, i, &string);

				std::string path(string.C_Str());
				size_t end = path.find_last_of('\\') + 1;
				path.erase(0, end);
				path.insert(0, directory);
				std::cout << type << " -> " << path << '\n';

				Texture texture;
				texture._id = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
				if(texture._id == 0) {
					std::cout << SOIL_last_result() << '\n';
				}
				texture._type = type_name.data();

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				mesh->_textures.push_back(std::move(texture));
			}
		};

		load_textures(aiTextureType_DIFFUSE, "Diffuse");
		load_textures(aiTextureType_SPECULAR, "Specular");
	}
}

Scene::Scene() :
	_parent		( nullptr ),
	_program	( nullptr )
{}

void Scene::draw(int mode, glm::mat4 transform) {
	auto model = transform * _transform.get_model();

	for(auto& mesh : _meshes) {
		if (_program) {
			mesh.draw(_program->_id, model, mode);
		}
	}

	for(auto& child : _children) {
		child->draw(mode, model);
	}
}

Scene* Scene::new_child() {
	auto child = std::make_unique<Scene>();
	child->_parent = this;
	_children.push_back(std::move(child));
	return _children.back().get();
}

void Scene::add_child(std::unique_ptr<Scene> child) {
	_children.push_back(std::move(child));
}

void Scene::attach_program(Program* program) {
	_program = program;

	for(auto& child : _children) {
		child->attach_program(program, child.get());
	}
}

void Scene::set_transform(Transform transform) {
	_transform = transform;
}

void Scene::attach_program(Program* program, Scene* scene) const {
	scene->_program = program;

	for (auto& child : scene->_children) {
		child->attach_program(program, child.get());
	}
}


bool Scene::load_assimp(std::string_view directory, std::string_view file) {
	Assimp::Importer importer;
	std::string path;
	path.reserve(directory.size() + file.size());
	path.append(directory);
	path.append(file);

	auto ai_scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
	if (!ai_scene) {
		std::cout << "Assimp Importer -> Couldn't load scene at -> " << path << '\n';
		std::cout << importer.GetErrorString();
		return false;
	}

	construct_scene_from_assimp(ai_scene, ai_scene->mRootNode, this, directory, "");

	return true;
}

void Scene::construct_scene_from_assimp(const aiScene* ai_scene, aiNode* node, Scene* scene, std::string_view directory, std::string depth) const {
	std::cout << depth << "> Node: " << node->mName.C_Str() << '\n';
	std::cout << depth << "> Children: " << node->mNumChildren << '\n';
	std::cout << depth << "> Meshes: " << node->mNumMeshes << '\n';

	for(unsigned int i = 0; i < node->mNumMeshes; ++i) {
		const auto ai_mesh = ai_scene->mMeshes[node->mMeshes[i]];
		const auto ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
		Mesh mesh;
		copy_mesh(ai_mesh, ai_material, &mesh, directory);
		scene->_meshes.push_back(std::move(mesh));
		scene->_meshes.back().init_buffers();
	}

	aiVector3D ai_position, ai_scale, ai_rotation;
	node->mTransformation.Decompose(ai_scale, ai_rotation, ai_position);
	const glm::vec3 position = glm::vec3(ai_position.x, ai_position.y, ai_position.z);
	const glm::vec3 scale = glm::vec3(ai_scale.x, ai_scale.y, ai_scale.z);
	const glm::vec3 rotation = glm::vec3(ai_rotation.x, ai_rotation.y, ai_rotation.z);
	scene->_transform.set_position(position);
	scene->_transform.set_scale(scale);
	scene->_transform.set_rotation(rotation);

	depth.push_back('-');
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		const auto child = scene->new_child();
		construct_scene_from_assimp(ai_scene, node->mChildren[i], child, directory, depth);
	}
}