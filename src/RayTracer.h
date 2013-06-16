#ifndef RAYTRACER_H_
#define RAYTRACER_H_

#include "Camera.h"
#include "Scene.h"

class RayTracer
{
public:
	RayTracer(const Camera* pCamera, const Scene* pScene);
	~RayTracer();

	void Render(unsigned char* pColorBuffer);

private:
	const Camera* mpCamera;
	const Scene* mpScene;

	Color3F BlinnPhong(const Material& rMaterial, const Light& rLight, const Vector3F& rLightDirection, const Vector3F& rViewerDirection, const Vector3F& rNormal) const;

};

#endif