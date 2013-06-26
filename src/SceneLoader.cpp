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

#include <string.h>
#include <vector>
#include <exception>

//////////////////////////////////////////////////////////////////////////
void SceneLoader::LoadFromXML(const std::string& rFileName, Scene& rScene, Camera& rCamera)
{
	if (rFileName.empty())
	{
		throw std::exception("empty scene file name");
	}

	char* pFileContent = FileReader::Read(rFileName, FileReader::FM_TEXT);

	if (pFileContent == 0)
	{
		throw std::exception("empty scene file");
	}

	rapidxml::xml_document<> doc;
	doc.parse<0>(pFileContent);

	std::map<int, SceneObject*> sceneObjectIds;
	std::map<int, int> sceneObjectParenting;
	rapidxml::xml_node<>* pRoot = doc.first_node();
	if (pRoot && strcmp("Scene", pRoot->name()) == 0)
	{
		for (rapidxml::xml_node<>* pChild = pRoot->first_node(); pChild; pChild = pChild->next_sibling())
		{
			Traverse(rScene, rCamera, sceneObjectIds, sceneObjectParenting, pChild);
		}

		std::map<int, SceneObject*>::iterator it = sceneObjectIds.begin();
		while (it != sceneObjectIds.end())
		{
			SceneObject* pSceneObject = it->second;
			int parentId = sceneObjectParenting[it->first];

			SceneObject* pParent = FindParent(sceneObjectIds, parentId);

			if (pParent != 0)
			{
				pSceneObject->parent = pParent;
				pParent->children.push_back(pSceneObject);
			}

			rScene.AddSceneObject(pSceneObject);

			it++;
		}
	}
	else
	{
		throw std::exception("invalid scene file");
	}
}

//////////////////////////////////////////////////////////////////////////
SceneObject* SceneLoader::FindParent(std::map<int, SceneObject*> rSceneObjectIds, int id)
{
	if (rSceneObjectIds.find(id) == rSceneObjectIds.end())
	{
		return 0;
	}

	return rSceneObjectIds[id];
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::Traverse(Scene& rScene, Camera& rCamera, std::map<int, SceneObject*>& rSceneObjectIds, std::map<int, int>& rSceneObjectParenting, rapidxml::xml_node<>* pXmlNode)
{
	if (strcmp("Camera", pXmlNode->name()) == 0)
	{
		ParseCamera(rCamera, pXmlNode);
	}
	else if (strcmp("Light", pXmlNode->name()) == 0)
	{
		ParseLight(rScene, pXmlNode);
	}
	else if (strcmp("Sphere", pXmlNode->name()) == 0)
	{
		ParseSphere(rScene, rSceneObjectIds, rSceneObjectParenting, pXmlNode);
	}
	else if (strcmp("Mesh", pXmlNode->name()) == 0)
	{
		ParseMesh(rScene, rSceneObjectIds, rSceneObjectParenting, pXmlNode);
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseCamera(Camera& rCamera, rapidxml::xml_node<>* pXmlNode)
{
	float fov = GetFloat(pXmlNode, "fov");
	float nearZ = GetFloat(pXmlNode, "near");
	float farZ = GetFloat(pXmlNode, "far");
	Vector3F& rEyePosition = GetVector3F(pXmlNode, "eyePosition");
	Vector3F& rForward = GetVector3F(pXmlNode, "forward");
	Vector3F& rUp = GetVector3F(pXmlNode, "up");

	rCamera.SetParameters(fov, nearZ, farZ, rEyePosition, rForward, rUp);
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseLight(Scene& rScene, rapidxml::xml_node<>* pXmlNode)
{
	char* pType = GetValue(pXmlNode, "type");

	ColorRGBA& rDiffuseColor = GetColorRGBA(pXmlNode, "diffuseColor");
	ColorRGBA& rSpecularColor = GetColorRGBA(pXmlNode, "specularColor");
	float intensity = GetFloat(pXmlNode, "intensity");

	if (strcmp("point", pType) == 0)
	{
		Vector3F& rPosition = GetVector3F(pXmlNode, "position");
		float attenuation = GetFloat(pXmlNode, "attenuation");

		rScene.AddLight(new PointLight(rDiffuseColor, rSpecularColor, intensity, rPosition, attenuation));
	}
	else if (strcmp("directional", pType) == 0)
	{
		Vector3F& rDirection = GetVector3F(pXmlNode, "direction");

		rScene.AddLight(new DirectionalLight(rDiffuseColor, rSpecularColor, intensity, rDirection));
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseSphere(Scene& rScene, std::map<int, SceneObject*>& rSceneObjectIds, std::map<int, int>& rSceneObjectParenting, rapidxml::xml_node<>* pXmlNode)
{
	float radius = GetFloat(pXmlNode, "radius");
	Sphere* pSphere = new Sphere(radius);

	int id = GetInt(pXmlNode, "id");
	int parentId = GetInt(pXmlNode, "parentId");

	rSceneObjectParenting[id] = parentId;

	for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild; pChild = pChild->next_sibling())
	{
		if (strcmp("Transform", pChild->name()) == 0)
		{
			ParseTransform(pChild, pSphere->localTransform);
		}
		else if (strcmp("Material", pChild->name()) == 0)
		{
			ParseMaterial(pChild, pSphere->material);
		}
	}

	rSceneObjectIds[id] = pSphere;
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseMesh(Scene& rScene, std::map<int, SceneObject*>& rSceneObjectIds, std::map<int, int>& rSceneObjectParenting, rapidxml::xml_node<>* pXmlNode)
{
	Mesh* pMesh = new Mesh();

	int id = GetInt(pXmlNode, "id");
	int parentId = GetInt(pXmlNode, "parentId");

	rSceneObjectParenting[id] = parentId;

	char* pVerticesFileName = GetValue(pXmlNode, "vertices");
	char* pNormalsFileName = GetValue(pXmlNode, "normals");
	char* pUVsFileName = GetValue(pXmlNode, "uvs");
	char* pIndicesFileName = GetValue(pXmlNode, "indices");

	ReadFileToVector(pVerticesFileName, pMesh->vertices);
	ReadFileToVector(pNormalsFileName, pMesh->normals);
	ReadFileToVector(pUVsFileName, pMesh->uvs);
	ReadFileToVector(pIndicesFileName, pMesh->indices);

	for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild; pChild = pChild->next_sibling())
	{
		if (strcmp("Transform", pChild->name()) == 0)
		{
			ParseTransform(pChild, pMesh->localTransform);
		}
		else if (strcmp("Material", pChild->name()) == 0)
		{
			ParseMaterial(pChild, pMesh->material);
		}
	}

	// TODO: generalize bounding volume creation
	BoundingSphere* pBoundingSphere = new BoundingSphere();
	pBoundingSphere->Compute(pMesh->vertices);
	pMesh->boundingVolume = pBoundingSphere;

	rSceneObjectIds[id] = pMesh;
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseTransform(rapidxml::xml_node<>* pXmlNode, Transform& rTransform)
{
	rTransform.scale = GetMatrix3x3F(pXmlNode, "scale");
	rTransform.rotation = GetMatrix3x3F(pXmlNode, "rotation");
	rTransform.position = GetVector3F(pXmlNode, "position");
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ParseMaterial(rapidxml::xml_node<>* pXmlNode, Material& rMaterial)
{
	rMaterial.ambientColor = GetColorRGBA(pXmlNode, "ambientColor");
	rMaterial.diffuseColor = GetColorRGBA(pXmlNode, "diffuseColor");
	rMaterial.specularColor = GetColorRGBA(pXmlNode, "specularColor");
	rMaterial.shininess = GetFloat(pXmlNode, "shininess");

	char* pTextureFileName = GetValue(pXmlNode, "texture");
	if (pTextureFileName)
	{
		rMaterial.texture = TextureLoader::LoadFromPNG(pTextureFileName);
	}

	rMaterial.transparent = GetBool(pXmlNode, "transparent");
	rMaterial.reflection = GetFloat(pXmlNode, "reflection");
	rMaterial.refraction = GetBool(pXmlNode, "refraction");
}

//////////////////////////////////////////////////////////////////////////
char* SceneLoader::GetValue(rapidxml::xml_node<>* pXmlNode, const char* pName)
{
	for (rapidxml::xml_attribute<>* attribute = pXmlNode->first_attribute(); attribute; attribute = attribute->next_attribute())
	{
		if (strcmp(pName, attribute->name()) == 0)
		{
			return attribute->value();
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
float SceneLoader::GetFloat(rapidxml::xml_node<>* pXmlNode, const char* pName)
{
	char* pFloat = GetValue(pXmlNode, pName);
	float f = 0;
	if (pFloat)
	{
		int n;
		n = sscanf(pFloat, "%f", &f);
	}

	return f;
}

//////////////////////////////////////////////////////////////////////////
int SceneLoader::GetInt(rapidxml::xml_node<>* pXmlNode, const char* pName)
{
	char* pInt = GetValue(pXmlNode, pName);
	int i = 0;
	if (pInt)
	{
		int n;
		n = sscanf(pInt, "%d", &i);
		if (n < 1)
		{
			throw std::exception("invalid attribute size");
		}
	}

	return i;
}

//////////////////////////////////////////////////////////////////////////
bool SceneLoader::GetBool(rapidxml::xml_node<>* pXmlNode, const char* pName)
{
	char* pBool = GetValue(pXmlNode, pName);
	unsigned int b = 0;
	if (pBool)
	{
		int n;
		n = sscanf(pBool, "%d", &b);
		if (n < 1)
		{
			throw std::exception("invalid attribute size");
		}
	}

	return (b != 0);
}

//////////////////////////////////////////////////////////////////////////
ColorRGBA SceneLoader::GetColorRGBA(rapidxml::xml_node<>* pXmlNode, const char* pName)
{
	ColorRGBA color = ColorRGBA::WHITE;
	char* pColor = GetValue(pXmlNode, pName);
	if (pColor)
	{
		int n;
		n = sscanf(pColor, "%f, %f, %f, %f", &color.r(), &color.g(), &color.b(), &color.a());
	}

	return color;
}

//////////////////////////////////////////////////////////////////////////
Vector3F SceneLoader::GetVector3F(rapidxml::xml_node<>* pXmlNode, const char* pName)
{
	Vector3F vector;
	char* pVector = GetValue(pXmlNode, pName);
	if (pVector)
	{
		int n;
		n = sscanf(pVector, "%f, %f, %f", &vector.x, &vector.y, &vector.z);
		if (n < 1)
		{
			throw std::exception("invalid attribute size");
		}
	}

	return vector;
}

//////////////////////////////////////////////////////////////////////////
Matrix3x3F SceneLoader::GetMatrix3x3F(rapidxml::xml_node<>* pXmlNode, const char* pName)
{
	float m11, m12, m13, m21, m22, m23, m31, m32, m33;
	char* pVector = GetValue(pXmlNode, pName);
	if (pVector)
	{
		int n;
		n = sscanf(pVector, "%f, %f, %f, %f, %f, %f, %f, %f, %f", &m11, &m12, &m13, &m21, &m22, &m23, &m31, &m32, &m33);
		if (n < 1)
		{
			throw std::exception("invalid attribute size");
		}
	}

	return Matrix3x3F(m11, m12, m13, m21, m22, m23, m31, m32, m33);
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ReadFileToVector(char* pFileName, std::vector<Vector3F>& rVector)
{
	char* pBuffer = FileReader::Read(pFileName, FileReader::FM_TEXT);

	std::vector<std::string> lines;
	StringUtils::Tokenize(pBuffer, "\n", lines);

	float x, y, z;
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		std::string& rLine = lines[i];

		sscanf(rLine.c_str(), "%f, %f, %f", &x, &y, &z);

		rVector.push_back(Vector3F(x, y, z));
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ReadFileToVector(char* pFileName, std::vector<Vector2F>& rVector)
{
	char* pBuffer = FileReader::Read(pFileName, FileReader::FM_TEXT);

	std::vector<std::string> lines;
	StringUtils::Tokenize(pBuffer, "\n", lines);

	float x, y;
	for (unsigned int i = 0; i < lines.size(); i++)
	{
		std::string& rLine = lines[i];
		
		sscanf(rLine.c_str(), "%f, %f", &x, &y);

		rVector.push_back(Vector2F(x, y));
	}
}

//////////////////////////////////////////////////////////////////////////
void SceneLoader::ReadFileToVector(char* pFileName, std::vector<unsigned int>& rVector)
{
	char* pBuffer = FileReader::Read(pFileName, FileReader::FM_TEXT);

	std::vector<std::string> values;
	StringUtils::Tokenize(pBuffer, " ", values);

	int index;
	for (unsigned int i = 0; i < values.size(); i++)
	{
		std::string& rValue = values[i];

		sscanf(rValue.c_str(), "%d", &index);

		if (index < 0)
		{
			throw std::exception("invalid index");
		}

		rVector.push_back((unsigned int)index);
	}
}