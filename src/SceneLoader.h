#ifndef SCENELOADER_H_
#define SCENELOADER_H_

#include "Scene.h"
#include "RapidXML.h"
#include "Camera.h"
#include "Material.h"
#include "Transform.h"
#include "ColorRGBA.h"
#include "Vector3F.h"
#include "Matrix3x3F.h"

#include <string>

class SceneLoader
{
public:
	static void LoadFromXML(const std::string& rFileName, Scene& rScene, Camera& rCamera);

private:
	SceneLoader() {}
	~SceneLoader() {}

	static void Traverse(Scene& rScene, Camera& rCamera, rapidxml::xml_node<>* pXmlNode);
	static void ParseCamera(Camera& rCamera, rapidxml::xml_node<>* pXmlNode);
	static void ParseLight(Scene& rScene, rapidxml::xml_node<>* pXmlNode);
	static void ParseSphere(Scene& rScenee, rapidxml::xml_node<>* pXmlNode);
	static void ParseMesh(Scene& rScene, rapidxml::xml_node<>* pXmlNode);
	static char* GetValue(rapidxml::xml_node<>* pXmlNode, const char* pName);
	static float GetFloat(rapidxml::xml_node<>* pXmlNode, const char* pName);
	static unsigned int GetUInt(rapidxml::xml_node<>* pXmlNode, const char* pName);
	static bool GetBool(rapidxml::xml_node<>* pXmlNode, const char* pName);
	static ColorRGBA GetColorRGBA(rapidxml::xml_node<>* pXmlNode, const char* pName);
	static Vector3F GetVector3F(rapidxml::xml_node<>* pXmlNode, const char* pName);
	static Matrix3x3F GetMatrix3x3F(rapidxml::xml_node<>* pXmlNode, const char* pName);
	static void ReadFileToVector(char* pFileName, std::vector<Vector3F>& rVector);
	static void ReadFileToVector(char* pFileName, std::vector<Vector2F>& rVector);
	static void ReadFileToVector(char* pFileName, std::vector<unsigned int>& rVector);
	static void ParseTransform(rapidxml::xml_node<>* pXmlNode, Transform& rTransform);
	static void ParseMaterial(rapidxml::xml_node<>* pXmlNode, Material& rMaterial);
	
};

#endif