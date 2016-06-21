#ifndef OPENGLRENDERER_H_
#define OPENGLRENDERER_H_

#include <memory>
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

	inline void SetDebugRay(const RayMetadata& rRayToDebug)
	{
		mpRayToDebug = &rRayToDebug;
	}

	inline void EnableDebugRay()
	{
		mDebugRayEnabled = true;
	}

	inline void DisableDebugRay()
	{
		mDebugRayEnabled = false;
	}

	virtual void Render();

private:
	static const float MISSED_RAY_LENGTH;

	std::map<unsigned int, unsigned int> mTextureIds;
	std::map<unsigned int, std::unique_ptr<Mesh>> mSphereMeshes;
	bool mDebugRayEnabled;
	const RayMetadata* mpRayToDebug;

	void RenderRay();
	void RenderMesh(unsigned int i, std::shared_ptr<Mesh>& mesh);
	void RenderSphere(unsigned int i, std::shared_ptr<Sphere>& sphere);
	void SetUpMaterial(unsigned int i, std::shared_ptr<SceneObject>& sceneObject);
	void RenderTriangles(const Matrix4F& model, const std::vector<unsigned int>& indices, const std::vector<Vector3F>& vertices, const std::vector<Vector3F>& normals, const std::vector<Vector2F>& uvs);
	unsigned int AllocateTextureForSceneObject(unsigned int i, std::shared_ptr<SceneObject>& pSceneObject);
	std::unique_ptr<Mesh> CreateMeshForSphere(std::shared_ptr<Sphere>& sphere);

};

#endif