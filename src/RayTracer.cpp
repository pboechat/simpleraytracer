#include "RayTracer.h"
#include "Ray.h"
#include "RayHit.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include <cmath>

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define clamp(x, vmin, vmax) (((x) < (vmin)) ? (vmin) : (((x) > (vmax)) ? (vmax) : (x)))

RayTracer::RayTracer(const Camera* pCamera, const Scene* pScene) :
	mpCamera(pCamera),
	mpScene(pScene)
{
}

RayTracer::~RayTracer()
{
}

void RayTracer::Render(unsigned char* pColorBuffer)
{
	for (unsigned int y = 0, c = 0; y < mpCamera->GetHeight(); y++)
	{
		for (unsigned int x = 0; x < mpCamera->GetWidth(); x++, c += 4)
		{
			Ray& rRay = mpCamera->CastRay(x, y);

			pColorBuffer[c] = 0;
			pColorBuffer[c + 1] = 0;
			pColorBuffer[c + 2] = 0;
			pColorBuffer[c + 3] = 255;

			for (unsigned int i = 0; i < mpScene->NumberOfSceneObjects(); i++)
			{
				SceneObject* pSceneObject = mpScene->GetSceneObject(i);

				RayHit hit;
				if (pSceneObject->Intersect(rRay, hit))
				{
					Vector3F viewerDirection = (rRay.origin - hit.point).Normalized();
					Vector3F& rNormal = hit.normal;

					Color3F color;
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

						Color3F colorContribution = BlinnPhong(pSceneObject->material, *pLight, lightDirection, viewerDirection, rNormal);

						if (pPointLight != 0)
						{
							float distance = pPointLight->position.Distance(hit.point);
							float distanceAttenuation = 1.0f / max(pPointLight->attenuation * (distance * distance), 1.0f);
							colorContribution *= distanceAttenuation;
						}

						color += colorContribution;
					}

					color.x = clamp(color.x, 0, 1);
					color.y = clamp(color.y, 0, 1);
					color.z = clamp(color.z, 0, 1);

					color *= 255.0f;

					pColorBuffer[c] = (unsigned char)color.x;
					pColorBuffer[c + 1] = (unsigned char)color.y;
					pColorBuffer[c + 2] = (unsigned char)color.z;
					pColorBuffer[c + 3] = 255;
				}
			}
		}
	}
}

Color3F RayTracer::BlinnPhong(const Material& rMaterial, const Light& rLight, const Vector3F& rLightDirection, const Vector3F& rViewerDirection, const Vector3F& rNormal) const
{
	Color3F ambientContribution = rLight.intensity * rLight.ambientColor * rMaterial.ambientColor;

	float diffuseAttenuation = max(rNormal.Dot(rViewerDirection), 0);

	Color3F diffuseContribution = rLight.intensity * rLight.diffuseColor * rMaterial.diffuseColor * diffuseAttenuation;
	
	Color3F specularContribution;
	if (diffuseAttenuation > 0)
	{
		Vector3F reflection = (-rLightDirection).Reflection(rNormal).Normalized();
		float specularAttenuation = pow(max(reflection.Dot(rViewerDirection), 0), rMaterial.shininess);
		specularContribution = rLight.intensity * rLight.specularColor * rMaterial.specularColor * specularAttenuation;
	}

	return (ambientContribution + diffuseContribution + specularContribution);
}