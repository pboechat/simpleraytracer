#ifndef MODEL_H_
#define MODEL_H_

#include <vector>
#include <string>
#include <memory>
#include <cassert>
#include <stdexcept>

#include "Scene.h"
#include "Mesh.h"
#include "TinyObjLoader.h"

using namespace TinyObjLoader;

struct ModelLoader
{
	static std::unique_ptr<Mesh> LoadObj(const std::string& fileName)
	{
		std::vector<shape_t> shapes;

		auto errorStr = TinyObjLoader::LoadObj(shapes, fileName.c_str());
		if (!errorStr.empty())
		{
			throw std::runtime_error("[TinyObjLoader] " + errorStr);
		}

		std::unique_ptr<Mesh> mesh(new Mesh());
		unsigned int offset = 0;
		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			shape_t& shape = shapes[i];

			assert(shape.mesh.indices.size() % 3 == 0);

			for (unsigned int j = 0; j < shape.mesh.indices.size(); j++)
			{
				mesh->indices.push_back(offset + shape.mesh.indices[j]);
			}

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

			offset += mesh->vertices.size();
		}

		return mesh;
	}

private:
	ModelLoader() = default;
	~ModelLoader() = default;

};

#endif