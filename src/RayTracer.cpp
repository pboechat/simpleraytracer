#include "RayTracer.h"
#include "Camera.h"
#include "RayHit.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"
#include "Vector3F.h"
#include "Vector4F.h"
#include "Matrix3x3F.h"
#include "Matrix4x4F.h"
#include "Application.h"

#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include "glext.h"
#include <cmath>
#include <exception>

#include "Common.h"

const unsigned int RayTracer::DEPTH_BUFFER_SIZE = Application::SCREEN_WIDTH * Application::SCREEN_HEIGHT;
const unsigned int RayTracer::COLOR_BUFFER_SIZE = DEPTH_BUFFER_SIZE * Application::BYTES_PER_PIXEL;
const unsigned int RayTracer::RAYS_METADATA_SIZE = Application::SCREEN_WIDTH * Application::SCREEN_HEIGHT;
const unsigned int RayTracer::MAX_ITERATIONS = 5;

#define clampColor(v, vmin, vmax) \
	(v).r() = (((v).r() < (vmin)) ? (vmin) : (((v).r() > (vmax)) ? (vmax) : (v).r())); \
	(v).g() = (((v).g() < (vmin)) ? (vmin) : (((v).g() > (vmax)) ? (vmax) : (v).g())); \
	(v).b() = (((v).b() < (vmin)) ? (vmin) : (((v).b() > (vmax)) ? (vmax) : (v).b())); \
	(v).a() = (((v).a() < (vmin)) ? (vmin) : (((v).a() > (vmax)) ? (vmax) : (v).a()))

#define setColor(colorBuffer, i, c) \
	(colorBuffer)[(i)] = static_cast<unsigned char>((c).r() * 255.0); \
	(colorBuffer)[(i) + 1] = static_cast<unsigned char>((c).g() * 255.0); \
	(colorBuffer)[(i) + 2] = static_cast<unsigned char>((c).b() * 255.0); \
	(colorBuffer)[(i) + 3] = 255

#define getColor(colorBuffer, i, c) \
	(c).r() = (colorBuffer)[(i)] / 255.0f; \
	(c).g() = (colorBuffer)[(i) + 1] / 255.0f; \
	(c).b() = (colorBuffer)[(i) + 2] / 255.0f; \
	(c).a() = (colorBuffer)[(i) + 3] / 255.0f

#ifdef _WIN32
PFNGLGENBUFFERSARBPROC pglGenBuffersARB = 0;
PFNGLBINDBUFFERARBPROC pglBindBufferARB = 0;
PFNGLBUFFERDATAARBPROC pglBufferDataARB = 0;
PFNGLBUFFERSUBDATAARBPROC pglBufferSubDataARB = 0;
PFNGLDELETEBUFFERSARBPROC pglDeleteBuffersARB = 0;
PFNGLGETBUFFERPARAMETERIVARBPROC pglGetBufferParameterivARB = 0;
PFNGLMAPBUFFERARBPROC pglMapBufferARB = 0;
PFNGLUNMAPBUFFERARBPROC pglUnmapBufferARB = 0;
#define glGenBuffersARB           pglGenBuffersARB
#define glBindBufferARB           pglBindBufferARB
#define glBufferDataARB           pglBufferDataARB
#define glBufferSubDataARB        pglBufferSubDataARB
#define glDeleteBuffersARB        pglDeleteBuffersARB
#define glGetBufferParameterivARB pglGetBufferParameterivARB
#define glMapBufferARB            pglMapBufferARB
#define glUnmapBufferARB          pglUnmapBufferARB
#endif

//////////////////////////////////////////////////////////////////////////
RayTracer::RayTracer() :
	mpRaysMetadata(0),
	mTextureId(0),
	mPBOId(0),
	mPBOSupported(false),
	mpTextureData(0),
	mpDepthBuffer(0)
{
}

//////////////////////////////////////////////////////////////////////////
RayTracer::~RayTracer()
{
	if (mTextureId != 0)
	{
		glDeleteTextures(1, &mTextureId);
		mTextureId = 0;
	}

	if (mPBOId != 0)
	{
		glDeleteBuffersARB(1, &mPBOId);
		mPBOId = 0;
	}

	if (mpDepthBuffer != 0)
	{
		delete[] mpDepthBuffer;
		mpDepthBuffer = 0;
	}

	if (mpRaysMetadata != 0)
	{
		delete[] mpRaysMetadata;
		mpRaysMetadata = 0;
	}
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::Start()
{
#ifdef _WIN32
	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
	glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
	glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
	glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

	if(glGenBuffersARB && glBindBufferARB && glBufferDataARB && glBufferSubDataARB && glMapBufferARB && glUnmapBufferARB && glDeleteBuffersARB && glGetBufferParameterivARB)
	{
		mPBOSupported = true;
	}
#else
	// TODO:
#endif

	if (mPBOSupported)
	{
		glGenBuffersARB(1, &mPBOId);
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, COLOR_BUFFER_SIZE, 0, GL_STREAM_DRAW_ARB);
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
		mpTextureData = 0;
	}
	else
	{
		mpTextureData = new unsigned char[COLOR_BUFFER_SIZE];
		memset(mpTextureData, 0, sizeof(unsigned char) * COLOR_BUFFER_SIZE);
	}

	mpDepthBuffer = new float[DEPTH_BUFFER_SIZE];

	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Application::SCREEN_WIDTH, Application::SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)mpTextureData);
	glBindTexture(GL_TEXTURE_2D, 0);

	mpRaysMetadata = new RayMetadata[RAYS_METADATA_SIZE];
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::OnSetScene()
{
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	if (mPBOSupported)
	{
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Application::SCREEN_WIDTH, Application::SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, (void*)mpTextureData);

	float zFar = mpScene->GetCamera()->zFar();
	for (unsigned int i = 0; i < DEPTH_BUFFER_SIZE; i++)
	{
		mpDepthBuffer[i] = zFar;
	}

	if (mPBOSupported)
	{
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
		glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, COLOR_BUFFER_SIZE, 0, GL_STREAM_DRAW_ARB);
		unsigned char* pColorBuffer = (unsigned char*) glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);

		if (pColorBuffer != 0)
		{
			TraceRays(pColorBuffer);
			glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
		}

		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	}
	else
	{
		TraceRays(mpTextureData);
	}
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::Render()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, mTextureId);
	if (mPBOSupported)
	{
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Application::SCREEN_WIDTH, Application::SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, (void*)mpTextureData);

	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, mTextureId);
	glColor4f(1, 1, 1, 1);

	// TODO: improve
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);   
		glNormal3f(0, 0, 1);
		glVertex3f(-1, -1, 0);

		glTexCoord2f(1, 0);   
		glNormal3f(0, 0, 1);
		glVertex3f(1, -1, 0);

		glTexCoord2f(1, 1);   
		glNormal3f(0, 0, 1);
		glVertex3f(1, 1, 0);

		glTexCoord2f(0, 1); 
		glNormal3f(0, 0, 1);
		glVertex3f(-1, 1, 0);
	glEnd();

	if (mPBOSupported)
	{
		glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	}
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::ResetRayMetadata(RayMetadata& rRayMetadata)
{
	rRayMetadata.isReflection = false;
	rRayMetadata.isRefraction = false;
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::SetRayMetadata(RayMetadata& rRayMetadata, const Vector3F& rayOrigin, const Vector3F& hitPoint) const
{
	rRayMetadata.start = rayOrigin;
	if (rRayMetadata.next != 0)
	{
		delete rRayMetadata.next;
	}
	rRayMetadata.next = 0;
	rRayMetadata.end = hitPoint;
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::TraceRays(unsigned char* pColorBuffer)
{
	for (unsigned int y = 0, colorBufferIndex = 0, depthBufferIndex = 0, rayMetadataIndex = 0; y < Application::SCREEN_HEIGHT; y++)
	{
		for (unsigned int x = 0; x < Application::SCREEN_WIDTH; x++, colorBufferIndex += 4, depthBufferIndex++, rayMetadataIndex++)
		{
			Ray& rRay = mpScene->GetCamera()->GetRayFromScreenCoordinates(x, y);
			ResetRayMetadata(mpRaysMetadata[rayMetadataIndex]);
			ColorRGBA color = TraceRay(rRay, mpRaysMetadata[rayMetadataIndex], &mpDepthBuffer[depthBufferIndex], 0);
			setColor(pColorBuffer, colorBufferIndex, color);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
ColorRGBA RayTracer::TraceRay(const Ray& rRay, RayMetadata& rRayMetadata, float* pCurrentDepth, unsigned int iteration, SceneObject* pIgnoreSceneObject) const
{
	ColorRGBA finalColor = Application::CLEAR_COLOR;

	if (iteration > MAX_ITERATIONS)
	{
		return finalColor;
	}

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

			SetRayMetadata(rRayMetadata, rRay.origin, hit.point);

			ColorRGBA currentColor = Shade(pSceneObject, rRay, hit, rRayMetadata, iteration);

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
ColorRGBA RayTracer::Shade(SceneObject* pSceneObject, const Ray &rRay, const RayHit& rHit, RayMetadata& rRayMetadata, unsigned int iteration) const
{
	Material& rMaterial = pSceneObject->material;
	const Camera* pCamera = mpScene->GetCamera();

	ColorRGBA color = Application::GLOBAL_AMBIENT_LIGHT * rMaterial.ambientColor;

	Vector3F viewerDirection = (rRay.origin - rHit.point).Normalized();
	const Vector3F& rNormal = rHit.normal;

	for (unsigned int j = 0; j < mpScene->NumberOfLights(); j++)
	{
		Light* pLight = mpScene->GetLight(j);

		float distanceToLight = -1;
		Vector3F lightDirection;
		if (is(pLight, DirectionalLight))
		{
			lightDirection = cast(pLight, DirectionalLight)->direction;
		}
		else if (is(pLight, PointLight))
		{
			Vector3F lightPosition = cast(pLight, PointLight)->position;
			lightDirection = (lightPosition - rHit.point).Normalized();
			distanceToLight = lightPosition.Distance(rHit.point);
		}
		else 
		{
			throw std::exception("unimplemented light type");
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
		float newDepth = pCamera->zFar();
		RayMetadata* pReflectionRayMetadata = new RayMetadata();
		pReflectionRayMetadata->start = reflectionRay.origin;
		pReflectionRayMetadata->isReflection = true;
		rRayMetadata.next = pReflectionRayMetadata;
		color += rMaterial.reflection * TraceRay(reflectionRay, *pReflectionRayMetadata, &newDepth, iteration + 1, pSceneObject);
	}
	else if (rMaterial.refraction > 0)
	{
		Vector3F& rVt = viewerDirection.Dot(rNormal) * rNormal - viewerDirection;
		float sinI = rVt.Length();
		float sinT = rMaterial.refraction * sinI;
		float cosT = sqrt(1.0f - (sinT * sinT));
		Vector3F& rT = (1.0f / rVt.Length()) * rVt;
		Vector3F& rRefractionDirection = sinT * rT + cosT * (-rNormal);

		Ray refractionRay(rHit.point, rRefractionDirection);
		float newDepth = pCamera->zFar();
		RayMetadata* pRefractionRayMetadata = new RayMetadata();
		pRefractionRayMetadata->isRefraction = true;
		pRefractionRayMetadata->start = refractionRay.origin;
		rRayMetadata.next = pRefractionRayMetadata;
		color = color.Blend(TraceRay(refractionRay, *pRefractionRayMetadata, &newDepth, iteration + 1, pSceneObject));
	}

	clampColor(color, 0, 1);

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


