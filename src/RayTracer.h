#ifndef RAYTRACER_H_
#define RAYTRACER_H_

#include "Camera.h"
#include "Scene.h"
#include "Ray.h"
#include "SceneObject.h"

class RayTracer
{
public:
	RayTracer(const Camera* pCamera, const Scene* pScene, const Vector3F& rClearColor, const Vector3F& rAmbientLight);
	~RayTracer();

	void Render(unsigned char* pColorBuffer, float* pDepthBuffer);

private:
	const Camera* mpCamera;
	const Scene* mpScene;
	Vector3F mClearColor;
	Vector3F mAmbientLight;

	bool UnderShadow(const Ray& rShadowRay, float distanceToLight, SceneObject* pOriginSceneObject);
	Color3F BlinnPhong(const Material& rMaterial, const Light& rLight, const Vector3F& rLightDirection, const Vector3F& rViewerDirection, const Vector3F& rNormal) const;

};

#endif