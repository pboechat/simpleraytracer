#ifndef MODEL_H_
#define MODEL_H_

#include "Scene.h"
#include "Mesh.h"
#include "TinyObjLoader.h"

#include <vector>
#include <string>
#include <memory>

using namespace TinyObjLoader;

struct ModelLoader
{
	static void LoadObj(const std::string& fileName, Scene& scene)
	{
		std::vector<shape_t> shapes;

		TinyObjLoader::LoadObj(shapes, fileName.c_str());

		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			shape_t& shape = shapes[i];
			std::shared_ptr<Mesh> mesh(new Mesh());

			mesh->indices = shape.mesh.indices;

			for (unsigned int j = 0; j < shape.mesh.positions.size(); j += 3)
			{
				mesh->vertices.push_back(Vector3F(shape.mesh.positions[j], shape.mesh.positions[j + 1], shape.mesh.positions[j + 2]));
			}

			for (unsigned int j = 0; j < shape.mesh.normals.size(); j += 3)
			{
				mesh->normals.push_back(Vector3F(shape.mesh.normals[j], shape.mesh.normals[j + 1], shape.mesh.normals[j + 2]));
			}

			for (unsigned int j = 0; j < shape.mesh.texcoords.size(); j += 2)
			{
				mesh->uvs.push_back(Vector2F(shape.mesh.texcoords[j], shape.mesh.texcoords[j + 1]));
			}

			scene.AddSceneObject(std::dynamic_pointer_cast<SceneObject>(mesh));
		}
	}

private:
	ModelLoader() {}
	~ModelLoader() {}

};

#endif