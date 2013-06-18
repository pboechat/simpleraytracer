#include "RayTracer.h"
#include "RayHit.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include <cmath>

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define clamp(vector, vmin, vmax) \
	vector.x = ((vector.x < (vmin)) ? (vmin) : ((vector.x > (vmax)) ? (vmax) : vector.x)); \
	vector.y = ((vector.y < (vmin)) ? (vmin) : ((vector.y > (vmax)) ? (vmax) : vector.y)); \
	vector.z = ((vector.z < (vmin)) ? (vmin) : ((vector.z > (vmax)) ? (vmax) : vector.z))

#define writeToColorBuffer(colorBuffer, i, color) \
	(colorBuffer)[(i)] = (unsigned char)(color.x * 255.0f); \
	(colorBuffer)[(i) + 1] = (unsigned char)(color.y * 255.0f); \
	(colorBuffer)[(i) + 2] = (unsigned char)(color.z * 255.0f); \
	(colorBuffer)[(i) + 3] = 255

RayTracer::RayTracer(const Camera* pCamera, const Scene* pScene, const Vector3F& rClearColor, const Vector3F& rAmbientLight) :
	mpCamera(pCamera),
	mpScene(pScene),
	mClearColor(rClearColor),
	mAmbientLight(rAmbientLight)
{
}

RayTracer::~RayTracer()
{
}

void RayTracer::Render(unsigned char* pColorBuffer, float* pDepthBuffer)
{
	for (unsigned int y = 0, c = 0, d = 0; y < mpCamera->GetHeight(); y++)
	{
		for (unsigned int x = 0; x < mpCamera->GetWidth(); x++, c += 4, d++)
		{
			Ray& rRay = mpCamera->CastRay(x, y);

			writeToColorBuffer(pColorBuffer, c, mClearColor);

			for (unsigned int i = 0; i < mpScene->NumberOfSceneObjects(); i++)
			{
				SceneObject* pSceneObject = mpScene->GetSceneObject(i);

				RayHit hit;
				
				if (pSceneObject->Intersect(rRay, hit))
				{
					float distanceToPoint = rRay.origin.Distance(hit.point);

					if (distanceToPoint >= pDepthBuffer[d])
					{
						continue;
					}
					
					pDepthBuffer[d] = distanceToPoint;

					Color3F color = mAmbientLight * pSceneObject->material.ambientColor;

					Vector3F viewerDirection = (rRay.origin - hit.point).Normalized();
					Vector3F& rNormal = hit.normal;

					for (unsigned int j = 0; j < mpScene->NumberOfLights(); j++)
					{
						Light* pLight = mpScene->GetLight(j);

						PointLight* pPointLight;
						
						Vector3F lightDirection;
						if ((pPointLight = static_cast<PointLight*>(pLight)) != 0)
						{
							lightDirection = (pPointLight->position - hit.point).Normalized();
						}
						else
						{
							// FIXME: rotate according to view
							lightDirection = static_cast<DirectionalLight*>(pLight)->direction;
						}

						float distanceToLight;
						if (pPointLight == 0)
						{
							distanceToLight = -1;
						}
						else 
						{
							distanceToLight = pPointLight->position.Distance(hit.point);
						}

						Ray shadowRay(hit.point, lightDirection);
						if (UnderShadow(shadowRay, distanceToLight, pSceneObject))
						{
							continue;
						}

						Color3F colorContribution = BlinnPhong(pSceneObject->material, *pLight, lightDirection, viewerDirection, rNormal);

						if (pPointLight != 0)
						{
							float distanceAttenuation = 1.0f / max(pPointLight->attenuation * (distanceToLight * distanceToLight), 1);
							colorContribution *= distanceAttenuation;
						}

						color += colorContribution;
					}

					clamp(color, 0, 1);

					writeToColorBuffer(pColorBuffer, c, color);
				}
			}
		}
	}
}

bool RayTracer::UnderShadow(const Ray& rShadowRay, float distanceToLight, SceneObject* pOriginSceneObject)
{
	for (unsigned int i = 0; i < mpScene->NumberOfSceneObjects(); i++)
	{
		SceneObject* pSceneObject = mpScene->GetSceneObject(i);

		if (pSceneObject == pOriginSceneObject)
		{
			continue;
		}

		RayHit hit;
		if (pSceneObject->Intersect(rShadowRay, hit))
		{
			if (distanceToLight == -1 )
			{
				return true;
			}
			else if (distanceToLight > hit.point.Distance(rShadowRay.origin))
			{
				return true;
			}
		}
	}

	return false;
}

Color3F RayTracer::BlinnPhong(const Material& rMaterial, const Light& rLight, const Vector3F& rLightDirection, const Vector3F& rViewerDirection, const Vector3F& rNormal) const
{
	float diffuseAttenuation = max(rNormal.Dot(rViewerDirection), 0);

	Color3F diffuseContribution = rLight.intensity * rLight.diffuseColor * rMaterial.diffuseColor * diffuseAttenuation;
	
	Color3F specularContribution;
	if (diffuseAttenuation > 0)
	{
		Vector3F reflection = (-rLightDirection).Reflection(rNormal).Normalized();
		float specularAttenuation = pow(max(reflection.Dot(rViewerDirection), 0), rMaterial.shininess);
		specularContribution = rLight.intensity * rLight.specularColor * rMaterial.specularColor * specularAttenuation;
	}

	return (diffuseContribution + specularContribution);
}