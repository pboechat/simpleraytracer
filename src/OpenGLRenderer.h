#ifndef OPENGLRENDERER_H_
#define OPENGLRENDERER_H_

#include <map>

#include "Renderer.h"
#include "SceneObject.h"
#include "Sphere.h"
#include "Mesh.h"
#include "ColorRGBA.h"
#include "Matrix4x4F.h"
#include "RayMetadata.h"

class OpenGLRenderer : public Renderer
{
public:
	OpenGLRenderer();
	virtual ~OpenGLRenderer();

	inline void EnableDebugRay(const RayMetadata& rRayToDebug)
	{
		mRayToDebug = rRayToDebug;
		mDebugRayEnabled = true;
	}

	inline void DisableDebugRay()
	{
		mDebugRayEnabled = false;
	}

	virtual void Render();

private:
	std::map<SceneObject*, unsigned int> mTextureIds;
	std::map<Sphere*, Mesh*> mSphereMeshes;
	bool mDebugRayEnabled;
	RayMetadata mRayToDebug;

	void RenderMesh(Mesh* pMesh);
	void RenderRay();
	void RenderSphere(Sphere* pSphere);
	void SetUpMaterial(SceneObject* pSceneObject);
	void RenderTriangles(const Matrix4F& model, const std::vector<unsigned int>& indices, const std::vector<Vector3F>& vertices, const std::vector<Vector3F>& normals, const std::vector<Vector2F>& uvs);
	unsigned int AllocateTextureForSceneObject(SceneObject* pSceneOBject);
	Mesh* CreateMeshForSphere(Sphere* pSphere);

};

#endif