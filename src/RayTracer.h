#ifndef RAYTRACER_H_
#define RAYTRACER_H_

#include "Camera.h"
#include "Scene.h"
#include "Ray.h"
#include "SceneObject.h"
#include "ColorRGBA.h"

class RayTracer
{
public:
	RayTracer(const Camera* pCamera, const Scene* pScene, const ColorRGBA& rClearColor, const ColorRGBA& rAmbientLight);
	~RayTracer();

	void Render(unsigned char* pColorBuffer, float* pDepthBuffer);

private:
	const Camera* mpCamera;
	const Scene* mpScene;
	ColorRGBA mBackgroundColor;
	ColorRGBA mAmbientLight;

	ColorRGBA Trace(const Ray& rRay, float* pCurrentDepth, SceneObject* pIgnoreSceneObject = 0) const;
	ColorRGBA Shade(SceneObject* pSceneObject, const Ray& rRay, const RayHit& rHit) const;

	bool IsLightBlocked(const Ray& rShadowRay, float distanceToLight, SceneObject* pOriginSceneObject) const;
	ColorRGBA BlinnPhong(const ColorRGBA& rMaterialDiffuseColor, const ColorRGBA& rMaterialSpecularColor, float materialShininess, const Light& rLight, const Vector3F& rLightDirection, const Vector3F& rViewerDirection, const Vector3F& rNormal) const;

};

#endif