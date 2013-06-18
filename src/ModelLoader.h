#ifndef MODEL_H_
#define MODEL_H_

#include "Scene.h"
#include "Mesh.h"
#include "TinyObjLoader.h"

#include <vector>
#include <string>

using namespace TinyObjLoader;

struct ModelLoader
{
	static void LoadObj(const std::string& rFileName, Scene& rScene)
	{
		std::vector<shape_t> shapes;

		TinyObjLoader::LoadObj(shapes, rFileName.c_str());

		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			shape_t& rShape = shapes[i];
			Mesh* pMesh = new Mesh();

			pMesh->indices = rShape.mesh.indices;

			for (unsigned int j = 0; j < rShape.mesh.positions.size(); j += 3)
			{
				pMesh->vertices.push_back(Vector3F(rShape.mesh.positions[j], rShape.mesh.positions[j + 1], rShape.mesh.positions[j + 2]));
			}

			for (unsigned int j = 0; j < rShape.mesh.normals.size(); j += 3)
			{
				pMesh->normals.push_back(Vector3F(rShape.mesh.normals[j], rShape.mesh.normals[j + 1], rShape.mesh.normals[j + 2]));
			}

			for (unsigned int j = 0; j < rShape.mesh.texcoords.size(); j += 2)
			{
				pMesh->uvs.push_back(Vector2F(rShape.mesh.texcoords[j], rShape.mesh.texcoords[j + 1]));
			}

			rScene.AddSceneObject(pMesh);
		}
	}

private:
	ModelLoader() {}
	~ModelLoader() {}

};

#endif