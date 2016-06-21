#ifndef SCENELOADER_H_
#define SCENELOADER_H_

#include <string>
#include <map>
#include <memory>

#include "Scene.h"
#include "RapidXML.h"
#include "Camera.h"
#include "Material.h"
#include "Transform.h"
#include "ColorRGBA.h"
#include "Vector3F.h"
#include "Matrix3x3F.h"
#include "SceneObject.h"

class SceneLoader
{
public:
	static std::unique_ptr<Scene> LoadFromXML(const std::string& rFileName);

private:
	SceneLoader() = default;
	~SceneLoader() = default;

	static void Traverse(std::unique_ptr<Scene>& scene, std::map<int, std::shared_ptr<SceneObject> >& sceneObjects, std::map<int, int>& rSceneObjectParenting, rapidxml::xml_node<>* xmlNode);
	static void ParseCamera(std::unique_ptr<Scene>& scene, rapidxml::xml_node<>* xmlNode);
	static void ParseLight(std::unique_ptr<Scene>& scene, rapidxml::xml_node<>* xmlNode);
	static void ParseSphere(std::unique_ptr<Scene>& scene, std::map<int, std::shared_ptr<SceneObject> >& sceneObjects, std::map<int, int>& rSceneObjectParenting, rapidxml::xml_node<>* xmlNode);
	static void ParseMesh(std::unique_ptr<Scene>& scene, std::map<int, std::shared_ptr<SceneObject> >& sceneObjects, std::map<int, int>& rSceneObjectParenting, rapidxml::xml_node<>* xmlNode);
	static std::string GetValue(rapidxml::xml_node<>* xmlNode, const std::string& name);
	static bool HasValue(rapidxml::xml_node<>* xmlNode, const std::string& name);
	static float GetFloat(rapidxml::xml_node<>* xmlNode, const std::string& name);
	static int GetInt(rapidxml::xml_node<>* xmlNode, const std::string& name);
	static bool GetBool(rapidxml::xml_node<>* xmlNode, const std::string& name);
	static ColorRGBA GetColorRGBA(rapidxml::xml_node<>* xmlNode, const std::string& name);
	static Vector3F GetVector3F(rapidxml::xml_node<>* xmlNode, const std::string& name);
	static Matrix3x3F GetMatrix3x3F(rapidxml::xml_node<>* xmlNode, const std::string& name);
	static void ReadFileToVector(const std::string& fileName, std::vector<Vector3F>& v);
	static void ReadFileToVector(const std::string& fileName, std::vector<Vector2F>& v);
	static void ReadFileToVector(const std::string& fileName, std::vector<unsigned int>& v);
	static void ParseTransform(rapidxml::xml_node<>* xmlNode, Transform& transform);
	static void ParseMaterial(rapidxml::xml_node<>* xmlNode, Material& material);

};

#endif