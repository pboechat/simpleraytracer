#ifndef RAYTRACER_H_
#define RAYTRACER_H_

#include <vector>

#include "Renderer.h"
#include "Scene.h"
#include "Ray.h"
#include "SceneObject.h"
#include "ColorRGBA.h"
#include "RayMetadata.h"

class RayTracer : public Renderer
{
public:
	RayTracer();
	virtual ~RayTracer();

	inline const std::unique_ptr<RayMetadata[]>& GetRaysMetadata() const
	{
		return mpRaysMetadata;
	}

	virtual void Start();
	virtual void Render();

protected:
	virtual void OnSetScene();

private:
	static const unsigned int DEPTH_BUFFER_SIZE;
	static const unsigned int COLOR_BUFFER_SIZE;
	static const unsigned int RAYS_METADATA_SIZE;
	static const unsigned int MAX_ITERATIONS;

	std::unique_ptr<RayMetadata[]> mpRaysMetadata;
	unsigned int mTextureId;
	unsigned int mPBOId;
	bool mPBOSupported;
	std::unique_ptr<unsigned char[]> mpTextureData;
	std::unique_ptr<float[]> mpDepthBuffer;

	void CreateBuffers();
	void TraceRays(std::unique_ptr<unsigned char[]>& colorBuffer);
	void ResetRayMetadata(RayMetadata& rRayMetadata);
	void SetRayMetadata(RayMetadata& rayMetadata, const Vector3F& rayOrigin, const Vector3F& hitPoint) const;
	ColorRGBA TraceRay(const Ray& rRay, RayMetadata& rRayMetadata, float* pCurrentDepth, unsigned int iteration, std::shared_ptr<SceneObject> pIgnoreSceneObject = std::shared_ptr<SceneObject>(nullptr)) const;
	ColorRGBA Shade(std::shared_ptr<SceneObject>& sceneObject, const Ray& rRay, const RayHit& rHit, RayMetadata& rRayMetadata, unsigned int iteration) const;
	bool IsLightBlocked(const Ray& rShadowRay, float distanceToLight, std::shared_ptr<SceneObject> origin) const;
	ColorRGBA BlinnPhong(const ColorRGBA& rMaterialDiffuseColor, const ColorRGBA& rMaterialSpecularColor, float materialShininess, const Light& rLight, const Vector3F& rLightDirection, const Vector3F& rViewerDirection, const Vector3F& rNormal) const;
	
};

#endif