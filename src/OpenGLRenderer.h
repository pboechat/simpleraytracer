#ifndef OPENGLRENDERER_H_
#define OPENGLRENDERER_H_

#include "Camera.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Sphere.h"
#include "Mesh.h"
#include "Matrix4x4F.h"

#include <map>

class OpenGLRenderer
{
public:
	OpenGLRenderer(const Camera* pCamera, const Scene* pScene);
	~OpenGLRenderer();

	void Render();
	
private:
	const Camera* mpCamera;
	const Scene* mpScene;
	std::map<SceneObject*, unsigned int> mTextureIds;
	std::map<Sphere*, Mesh*> mSphereMeshes;

	void RenderMesh(Mesh* pMesh);
	void RenderSphere(Sphere* pSphere);
	void SetUpMaterial(SceneObject* pSceneObject);
	void RenderTriangles(const Matrix4F& model, const std::vector<unsigned int>& indices, const std::vector<Vector3F>& vertices, const std::vector<Vector3F>& normals, const std::vector<Vector2F>& uvs);
	unsigned int AllocateTextureForSceneObject(SceneObject* pSceneOBject);
	Mesh* CreateMeshForSphere(Sphere* pSphere);

};

#endif