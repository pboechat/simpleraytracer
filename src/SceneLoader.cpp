#include "SceneLoader.h"
#include "FileReader.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Sphere.h"
#include "Mesh.h"
#include "Texture.h"
#include "TextureLoader.h"
#include "StringUtils.h"
#include "BoundingSphere.h"
#include "OBB.h"

#include <string.h>
#include <vector>
#include <exception>
#include <memory>

//////////////////////////////////////////////////////////////////////////
std::unique_ptr<Scene> SceneLoader::LoadFromXML(const std::string& fileName)
{
	if (fileName.empty())
	{
		throw std::exception("empty scene file name");
	}

	std::string sceneFileContent = FileReader::Read(fileName, FileReader::FM_TEXT);

	if (sceneFileContent.empty())
	{
		throw std::exception("empty scene file");
	}

	rapidxml::xml_document<> doc;
	doc.parse<0>(const_cast<char*>(sceneFileContent.c_str()));

	std::unique_ptr<Scene> scene(new Scene());
	std::map<int, std::shared_ptr<SceneObject> > sceneObjects;
	std::map<int, int> sceneObjectParenting;
	auto* root = doc.first_node();
	if (root && strcmp("Scene", root->name()) == 0)
	{
		for (auto* child = root->first_node(); child; child = child->next_sibling())
		{
			Traverse(scene, sceneObjects, sceneObjectParenting, child);
		}

		auto it = sceneObjects.begin();
		while (it != sceneObjects.end())
		{
			auto sceneObject = it->second;
			int parentId = sceneObjectParenting[it->first];
			if (sceneObjects.find(parentId) != sceneObjects.end())
			{
				auto parentSceneObject = sceneObjects[parentId];
				sceneObject->parent = parentSceneObject;
				parentSceneObject->children.push_back(sceneObject);
			}
			scene->AddSceneObject(sceneObject);
			it++;
		}
	}
	else
	{
		throw std::exception("invalid scene file");
	}

	return scene;
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::Traverse(std::unique_ptr<Scene>& scene, std::map<int, std::shared_ptr<SceneObject> >& sceneObjects, std::map<int, int>& sceneObjectParenting, rapidxml::xml_node<>* xmlNode)
{
	if (strcmp("Camera", xmlNode->name()) == 0)
	{
		ParseCamera(scene, xmlNode);
	}
	else if (strcmp("Light", xmlNode->name()) == 0)
	{
		ParseLight(scene, xmlNode);
	}
	else if (strcmp("Sphere", xmlNode->name()) == 0)
	{
		ParseSphere(scene, sceneObjects, sceneObjectParenting, xmlNode);
	}
	else if (strcmp("Mesh", xmlNode->name()) == 0)
	{
		ParseMesh(scene, sceneObjects, sceneObjectParenting, xmlNode);
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseCamera(std::unique_ptr<Scene>& scene, rapidxml::xml_node<>* xmlNode)
{
	float fov = GetFloat(xmlNode, "fov");
	float nearZ = GetFloat(xmlNode, "near");
	float farZ = GetFloat(xmlNode, "far");
	Vector3F rEyePosition = GetVector3F(xmlNode, "eyePosition");
	Vector3F rForward = GetVector3F(xmlNode, "forward");
	Vector3F rUp = GetVector3F(xmlNode, "up");

	std::unique_ptr<Camera> camera(new Camera(fov, nearZ, farZ));
	camera->localTransform.position = rEyePosition;
	camera->localTransform.LookAt(rForward, rUp);

	scene->SetCamera(camera);
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseLight(std::unique_ptr<Scene>& scene, rapidxml::xml_node<>* xmlNode)
{
	std::string type = GetValue(xmlNode, "type");

	ColorRGBA& rDiffuseColor = GetColorRGBA(xmlNode, "diffuseColor");
	ColorRGBA& rSpecularColor = GetColorRGBA(xmlNode, "specularColor");
	float intensity = GetFloat(xmlNode, "intensity");

	if (type == "point")
	{
		Vector3F& rPosition = GetVector3F(xmlNode, "position");
		float attenuation = GetFloat(xmlNode, "attenuation");

		scene->AddLight(std::unique_ptr<Light>(new PointLight(rDiffuseColor, rSpecularColor, intensity, rPosition, attenuation)));
	}
	else if (type == "directional")
	{
		Vector3F& rDirection = GetVector3F(xmlNode, "direction");

		scene->AddLight(std::unique_ptr<Light>(new DirectionalLight(rDiffuseColor, rSpecularColor, intensity, rDirection)));
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseSphere(std::unique_ptr<Scene>& scene, std::map<int, std::shared_ptr<SceneObject> >& sceneObjects, std::map<int, int>& sceneObjectParenting, rapidxml::xml_node<>* xmlNode)
{
	float radius = GetFloat(xmlNode, "radius");
	std::shared_ptr<SceneObject> sphere(new Sphere(radius));

	int id = GetInt(xmlNode, "id");
	int parentId = GetInt(xmlNode, "parentId");

	sceneObjectParenting[id] = parentId;

	for (rapidxml::xml_node<>* pChild = xmlNode->first_node(); pChild; pChild = pChild->next_sibling())
	{
		if (strcmp("Transform", pChild->name()) == 0)
		{
			ParseTransform(pChild, sphere->localTransform);
		}
		else if (strcmp("Material", pChild->name()) == 0)
		{
			ParseMaterial(pChild, sphere->material);
		}
	}

	sceneObjects[id] = sphere;
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseMesh(std::unique_ptr<Scene>& scene, std::map<int, std::shared_ptr<SceneObject> >& sceneObjects, std::map<int, int>& sceneObjectParenting, rapidxml::xml_node<>* xmlNode)
{
	std::shared_ptr<Mesh> mesh(new Mesh());

	int id = GetInt(xmlNode, "id");
	int parentId = GetInt(xmlNode, "parentId");

	sceneObjectParenting[id] = parentId;

	std::string verticesFileName = GetValue(xmlNode, "vertices");
	std::string pNormalsFileName = GetValue(xmlNode, "normals");
	std::string uvsFileName = GetValue(xmlNode, "uvs");
	std::string indicesFileName = GetValue(xmlNode, "indices");

	ReadFileToVector(verticesFileName, mesh->vertices);
	ReadFileToVector(pNormalsFileName, mesh->normals);
	ReadFileToVector(uvsFileName, mesh->uvs);
	ReadFileToVector(indicesFileName, mesh->indices);

	for (auto* child = xmlNode->first_node(); child; child = child->next_sibling())
	{
		if (strcmp("Transform", child->name()) == 0)
		{
			ParseTransform(child, mesh->localTransform);
		}
		else if (strcmp("Material", child->name()) == 0)
		{
			ParseMaterial(child, mesh->material);
		}
	}

	// TODO: generalize bounding volume creation
	std::unique_ptr<BoundingVolume> pBoundingVolume(new BoundingSphere());
	pBoundingVolume->Compute(mesh->vertices);
	mesh->boundingVolume = std::move(pBoundingVolume);

	sceneObjects[id] = mesh;
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseTransform(rapidxml::xml_node<>* xmlNode, Transform& transform)
{
	transform.scale = GetMatrix3x3F(xmlNode, "scale");
	transform.rotation = GetMatrix3x3F(xmlNode, "rotation");
	transform.position = GetVector3F(xmlNode, "position");
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseMaterial(rapidxml::xml_node<>* xmlNode, Material& material)
{
	material.ambientColor = GetColorRGBA(xmlNode, "ambientColor");
	material.diffuseColor = GetColorRGBA(xmlNode, "diffuseColor");
	material.specularColor = GetColorRGBA(xmlNode, "specularColor");
	material.shininess = GetFloat(xmlNode, "shininess");

	std::string textureFileName = GetValue(xmlNode, "texture");
	if (!textureFileName.empty())
	{
		material.texture = TextureLoader::LoadFromPNG(textureFileName);
	}

	material.transparent = GetBool(xmlNode, "transparent");
	material.reflection = GetFloat(xmlNode, "reflection");
	material.refraction = GetFloat(xmlNode, "refraction");
}

//////////////////////////////////////////////////////////////////////////
std::string SceneLoader::GetValue(rapidxml::xml_node<>* xmlNode, const std::string& name)
{
	for (rapidxml::xml_attribute<>* attribute = xmlNode->first_attribute(); attribute; attribute = attribute->next_attribute())
	{
		if (name == attribute->name())
		{
			return attribute->value();
		}
	}

	return "";
}

//////////////////////////////////////////////////////////////////////////
float SceneLoader::GetFloat(rapidxml::xml_node<>* xmlNode, const std::string& name)
{
	float f = 0;
	std::string floatStr = GetValue(xmlNode, name);
	if (!floatStr.empty())
	{
		int n;
		n = sscanf(floatStr.c_str(), "%f", &f);
	}

	return f;
}

//////////////////////////////////////////////////////////////////////////
int SceneLoader::GetInt(rapidxml::xml_node<>* xmlNode, const std::string& name)
{
	int i = 0;
	std::string intStr = GetValue(xmlNode, name);
	if (!intStr.empty())
	{
		int n;
		n = sscanf(intStr.c_str(), "%d", &i);
		if (n < 1)
		{
			throw std::exception("invalid attribute size");
		}
	}

	return i;
}

//////////////////////////////////////////////////////////////////////////
bool SceneLoader::GetBool(rapidxml::xml_node<>* xmlNode, const std::string& name)
{
	unsigned int b = 0;
	std::string boolStr = GetValue(xmlNode, name);
	if (!boolStr.empty())
	{
		int n;
		n = sscanf(boolStr.c_str(), "%d", &b);
		if (n < 1)
		{
			throw std::exception("invalid attribute size");
		}
	}

	return (b != 0);
}

//////////////////////////////////////////////////////////////////////////
ColorRGBA SceneLoader::GetColorRGBA(rapidxml::xml_node<>* xmlNode, const std::string& name)
{
	ColorRGBA color = ColorRGBA::WHITE;
	std::string colorStr = GetValue(xmlNode, name);
	if (!colorStr.empty())
	{
		int n;
		n = sscanf(colorStr.c_str(), "%f, %f, %f, %f", &color.r(), &color.g(), &color.b(), &color.a());
	}

	return color;
}

//////////////////////////////////////////////////////////////////////////
Vector3F SceneLoader::GetVector3F(rapidxml::xml_node<>* xmlNode, const std::string& name)
{
	Vector3F vector;
	std::string vectorStr = GetValue(xmlNode, name);
	if (!vectorStr.empty())
	{
		int n;
		n = sscanf(vectorStr.c_str(), "%f, %f, %f", &vector.x(), &vector.y(), &vector.z());
		if (n < 1)
		{
			throw std::exception("invalid attribute size");
		}
	}

	return vector;
}

//////////////////////////////////////////////////////////////////////////
Matrix3x3F SceneLoader::GetMatrix3x3F(rapidxml::xml_node<>* xmlNode, const std::string& name)
{
	float m11, m12, m13, m21, m22, m23, m31, m32, m33;
	std::string vectorStr = GetValue(xmlNode, name);
	if (!vectorStr.empty())
	{
		int n;
		n = sscanf(vectorStr.c_str(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", &m11, &m12, &m13, &m21, &m22, &m23, &m31, &m32, &m33);
		if (n < 1)
		{
			throw std::exception("invalid attribute size");
		}
	}

	return Matrix3x3F(m11, m12, m13, m21, m22, m23, m31, m32, m33);
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ReadFileToVector(const std::string& fileName, std::vector<Vector3F>& v)
{
	std::string buffer = FileReader::Read(fileName, FileReader::FM_TEXT);
	std::vector<std::string> lines;
	StringUtils::Tokenize(buffer, "\n", lines);
	float x, y, z;
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		std::string& line = lines[i];
		sscanf(line.c_str(), "%f, %f, %f", &x, &y, &z);
		v.push_back(Vector3F(x, y, z));
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ReadFileToVector(const std::string& fileName, std::vector<Vector2F>& v)
{
	std::string buffer = FileReader::Read(fileName, FileReader::FM_TEXT);
	std::vector<std::string> lines;
	StringUtils::Tokenize(buffer, "\n", lines);
	float x, y;
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		std::string& rLine = lines[i];
		sscanf(rLine.c_str(), "%f, %f", &x, &y);
		v.push_back(Vector2F(x, y));
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ReadFileToVector(const std::string& fileName, std::vector<unsigned int>& v)
{
	std::string buffer = FileReader::Read(fileName, FileReader::FM_TEXT);
	std::vector<std::string> values;
	StringUtils::Tokenize(buffer, " ", values);
	int index;
	for (unsigned int i = 0; i < values.size(); i++)
	{
		std::string& rValue = values[i];
		sscanf(rValue.c_str(), "%d", &index);
		if (index < 0)
		{
			throw std::exception("invalid index");
		}
		v.push_back((unsigned int)index);
	}
}