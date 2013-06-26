#include "RayTracer.h"
#include "RayHit.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"
#include "Vector3F.h"
#include "Vector4F.h"
#include "Matrix3x3F.h"
#include "Matrix4x4F.h"

#include <cmath>
#include <exception>

#define cast(x, y) dynamic_cast<y*>(x)
#define is(x, y) dynamic_cast<y*>(x) != 0

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

#define clamp(v, vmin, vmax) \
	(v).r() = (((v).r() < (vmin)) ? (vmin) : (((v).r() > (vmax)) ? (vmax) : (v).r())); \
	(v).g() = (((v).g() < (vmin)) ? (vmin) : (((v).g() > (vmax)) ? (vmax) : (v).g())); \
	(v).b() = (((v).b() < (vmin)) ? (vmin) : (((v).b() > (vmax)) ? (vmax) : (v).b())); \
	(v).a() = (((v).a() < (vmin)) ? (vmin) : (((v).a() > (vmax)) ? (vmax) : (v).a()))

#define set(colorBuffer, i, c) \
	(colorBuffer)[(i)] = static_cast<unsigned char>((c).r() * 255.0); \
	(colorBuffer)[(i) + 1] = static_cast<unsigned char>((c).g() * 255.0); \
	(colorBuffer)[(i) + 2] = static_cast<unsigned char>((c).b() * 255.0); \
	(colorBuffer)[(i) + 3] = 255

#define get(colorBuffer, i, c) \
	(c).r() = (colorBuffer)[(i)] / 255.0f; \
	(c).g() = (colorBuffer)[(i) + 1] / 255.0f; \
	(c).b() = (colorBuffer)[(i) + 2] / 255.0f; \
	(c).a() = (colorBuffer)[(i) + 3] / 255.0f

//////////////////////////////////////////////////////////////////////////
RayTracer::RayTracer(const Camera* pCamera, const Scene* pScene, const ColorRGBA& rClearColor, const ColorRGBA& rAmbientLight) :
	mpCamera(pCamera),
	mpScene(pScene),
	mBackgroundColor(rClearColor),
	mAmbientLight(rAmbientLight)
{
}

//////////////////////////////////////////////////////////////////////////
RayTracer::~RayTracer()
{
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::Render(unsigned char* pColorBuffer, float* pDepthBuffer)
{
	for (unsigned int y = 0, colorBufferIndex = 0, depthBufferIndex = 0; y < mpCamera->GetHeight(); y++)
	{
		for (unsigned int x = 0; x < mpCamera->GetWidth(); x++, colorBufferIndex += 4, depthBufferIndex++)
		{
			Ray& rRay = mpCamera->GetRayFromScreenCoordinates(x, y);

			ColorRGBA color;
			color = Trace(rRay, &pDepthBuffer[depthBufferIndex]);
			set(pColorBuffer, colorBufferIndex, color);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
ColorRGBA RayTracer::Trace(const Ray& rRay, float* pCurrentDepth, SceneObject* pIgnoreSceneObject) const
{
	ColorRGBA finalColor = mBackgroundColor;

	for (unsigned int i = 0; i < mpScene->NumberOfSceneObjects(); i++)
	{
		SceneObject* pSceneObject = mpScene->GetSceneObject(i);

		if (pIgnoreSceneObject != 0 && pSceneObject == pIgnoreSceneObject)
		{
			continue;
		}

		RayHit hit;
		if (pSceneObject->Intersect(rRay, hit))
		{
			float depth = rRay.origin.Distance(hit.point);

			if (depth >= *pCurrentDepth)
			{
				continue;
			}

			*pCurrentDepth = depth;

			ColorRGBA currentColor = Shade(pSceneObject, rRay, hit);

			if (pSceneObject->material.transparent)
			{
				finalColor = currentColor.Blend(finalColor);
			}
			else
			{
				finalColor = currentColor;
			}
		}
	}

	return finalColor;
}


//////////////////////////////////////////////////////////////////////////
ColorRGBA RayTracer::Shade(SceneObject* pSceneObject, const Ray &rRay, const RayHit& rHit) const
{
	Material& rMaterial = pSceneObject->material;

	ColorRGBA color = mAmbientLight * rMaterial.ambientColor;

	Vector3F viewerDirection = (rRay.origin - rHit.point).Normalized();
	const Vector3F& rNormal = rHit.normal;

	for (unsigned int j = 0; j < mpScene->NumberOfLights(); j++)
	{
		Light* pLight = mpScene->GetLight(j);

		Vector3F lightDirection;
		if (is(pLight, DirectionalLight))
		{
			Vector4F direction;
			ToVector4F(direction, cast(pLight, DirectionalLight)->direction);
			lightDirection = Vector3F(mpCamera->GetInverseRotationMatrix() * direction);
		}
		else if (is(pLight, PointLight))
		{
			lightDirection = (cast(pLight, PointLight)->position - rHit.point).Normalized();
		}
		else 
		{
			throw std::exception("unimplemented light type");
		}

		float distanceToLight = -1;
		if (is(pLight, PointLight))
		{
			Vector4F lightPosition;
			ToVector4F(lightPosition, cast(pLight, PointLight)->position);
			lightPosition = mpCamera->GetViewMatrix() * lightPosition;
			distanceToLight = Vector3F(lightPosition).Distance(rHit.point);
		}

		Ray shadowRay(rHit.point, lightDirection);
		if (IsLightBlocked(shadowRay, distanceToLight, pSceneObject))
		{
			continue;
		}

		ColorRGBA diffuseColor = rMaterial.diffuseColor;
		if (rMaterial.texture != 0)
		{
			diffuseColor *= rMaterial.texture->Fetch(rHit.uv);
		}

		ColorRGBA colorContribution = BlinnPhong(diffuseColor, rMaterial.specularColor, rMaterial.shininess, *pLight, lightDirection, viewerDirection, rNormal);

		if (is(pLight, PointLight))
		{
			float distanceAttenuation = 1.0f / max(cast(pLight, PointLight)->attenuation * (distanceToLight * distanceToLight), 1);
			colorContribution *= distanceAttenuation;
		}

		color += colorContribution;
	}

	if (rMaterial.reflection > 0)
	{
		Vector3F reflectionDirection = (-viewerDirection).Reflection(rNormal).Normalized();
		Ray reflectionRay(rHit.point, reflectionDirection);
		float newDepth = mpCamera->GetFar();
		color += rMaterial.reflection * Trace(reflectionRay, &newDepth, pSceneObject);
	}

	if (rMaterial.refraction > 0)
	{
		Vector3F& rVt = viewerDirection.Dot(rNormal) * rNormal - viewerDirection;
		float sinI = rVt.Length();
		float sinT = rMaterial.refraction * sinI;
		float cosT = sqrt(1.0f - (sinT * sinT));
		Vector3F& rT = (1.0f / rVt.Length()) * rVt;
		Vector3F& rRefractionDirection = sinT * rT + cosT * (-rNormal);

		Ray refractionRay(rHit.point, rRefractionDirection);
		float newDepth = mpCamera->GetFar();
		color = color.Blend(Trace(refractionRay, &newDepth, pSceneObject));
	}

	clamp(color, 0, 1);

	return color;
}

//////////////////////////////////////////////////////////////////////////
bool RayTracer::IsLightBlocked(const Ray& rShadowRay, float distanceToLight, SceneObject* pOriginSceneObject) const
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

//////////////////////////////////////////////////////////////////////////
ColorRGBA RayTracer::BlinnPhong(const ColorRGBA& rMaterialDiffuseColor, const ColorRGBA& rMaterialSpecularColor, float materialShininess, const Light& rLight, const Vector3F& rLightDirection, const Vector3F& rViewerDirection, const Vector3F& rNormal) const
{
	float diffuseAttenuation = max(rNormal.Dot(rViewerDirection), 0);

	ColorRGBA diffuseContribution = rLight.intensity * rLight.diffuseColor * rMaterialDiffuseColor * diffuseAttenuation;
	
	ColorRGBA specularContribution;
	if (diffuseAttenuation > 0)
	{
		Vector3F lightReflectionDirection = (-rLightDirection).Reflection(rNormal).Normalized();
		float specularAttenuation = pow(max(lightReflectionDirection.Dot(rViewerDirection), 0), materialShininess);
		specularContribution = rLight.intensity * rLight.specularColor * rMaterialSpecularColor * specularAttenuation;
	}

	return (diffuseContribution + specularContribution);
}
