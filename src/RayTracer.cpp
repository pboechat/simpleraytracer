#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include "glext.h"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>

#include "Common.h"
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
#include "SimpleRayTracerApp.h"
#include "Time.h"

const unsigned int RayTracer::DEPTH_BUFFER_SIZE = SimpleRayTracerApp::SCREEN_WIDTH * SimpleRayTracerApp::SCREEN_HEIGHT;
const unsigned int RayTracer::COLOR_BUFFER_SIZE = DEPTH_BUFFER_SIZE * SimpleRayTracerApp::BYTES_PER_PIXEL;
const unsigned int RayTracer::RAYS_METADATA_SIZE = SimpleRayTracerApp::SCREEN_WIDTH * SimpleRayTracerApp::SCREEN_HEIGHT;
const unsigned int RayTracer::MAX_ITERATIONS = 5;

#define srt_clampColor(v, vmin, vmax) \
	(v).r() = (((v).r() < (vmin)) ? (vmin) : (((v).r() > (vmax)) ? (vmax) : (v).r())); \
	(v).g() = (((v).g() < (vmin)) ? (vmin) : (((v).g() > (vmax)) ? (vmax) : (v).g())); \
	(v).b() = (((v).b() < (vmin)) ? (vmin) : (((v).b() > (vmax)) ? (vmax) : (v).b())); \
	(v).a() = (((v).a() < (vmin)) ? (vmin) : (((v).a() > (vmax)) ? (vmax) : (v).a()))

#define srt_setColor(colorBuffer, i, c) \
	(colorBuffer)[(i)] = static_cast<unsigned char>((c).r() * 255.0); \
	(colorBuffer)[(i) + 1] = static_cast<unsigned char>((c).g() * 255.0); \
	(colorBuffer)[(i) + 2] = static_cast<unsigned char>((c).b() * 255.0); \
	(colorBuffer)[(i) + 3] = 255

#define srt_getColor(colorBuffer, i, c) \
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
#define srt_glGenBuffers           pglGenBuffersARB
#define srt_glBindBuffer           pglBindBufferARB
#define srt_glBufferData           pglBufferDataARB
#define srt_glBufferSubData        pglBufferSubDataARB
#define srt_glDeleteBuffers        pglDeleteBuffersARB
#define srt_glGetBufferParameteriv pglGetBufferParameterivARB
#define srt_glMapBuffer            pglMapBufferARB
#define srt_glUnmapBuffer          pglUnmapBufferARB
#endif

//////////////////////////////////////////////////////////////////////////
RayTracer::RayTracer() :
	mpRaysMetadata(nullptr),
	mTextureId(0),
	mPBOId(0),
	mPBOSupported(false),
	mpTextureData(nullptr),
	mpDepthBuffer(nullptr)
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
		srt_glDeleteBuffers(1, &mPBOId);
		mPBOId = 0;
	}

	mpTextureData = nullptr;
	mpDepthBuffer = nullptr;
	mpRaysMetadata = nullptr;
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::Start()
{
#ifdef _WIN32
	srt_glGenBuffers = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
	srt_glBindBuffer = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
	srt_glBufferData = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
	srt_glBufferSubData = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
	srt_glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
	srt_glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
	srt_glMapBuffer = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	srt_glUnmapBuffer = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");

	if(srt_glGenBuffers && srt_glBindBuffer && srt_glBufferData && srt_glBufferSubData && srt_glMapBuffer && srt_glUnmapBuffer && srt_glDeleteBuffers && srt_glGetBufferParameteriv)
	{
		mPBOSupported = true;
	}
#else
#error current platform is not supported
#endif

	if (mPBOSupported)
	{
		srt_glGenBuffers(1, &mPBOId);
		srt_glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
		srt_glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, COLOR_BUFFER_SIZE, 0, GL_STREAM_DRAW_ARB);
		srt_glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
		mpTextureData = nullptr;
	}
	else
	{
		mpTextureData = std::unique_ptr<unsigned char[]>(new unsigned char[COLOR_BUFFER_SIZE]);
		// TODO:
		memset(mpTextureData.get(), 0, sizeof(unsigned char) * COLOR_BUFFER_SIZE);
	}

	mpDepthBuffer = std::unique_ptr<float[]>(new float[DEPTH_BUFFER_SIZE]);

	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SimpleRayTracerApp::SCREEN_WIDTH, SimpleRayTracerApp::SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)mpTextureData.get());
	glBindTexture(GL_TEXTURE_2D, 0);

	mpRaysMetadata = std::unique_ptr<RayMetadata[]>(new RayMetadata[RAYS_METADATA_SIZE]);
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::OnSetScene()
{
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	if (mPBOSupported)
	{
		srt_glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SimpleRayTracerApp::SCREEN_WIDTH, SimpleRayTracerApp::SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, (void*)mpTextureData.get());

	float zFar = mScene->GetCamera()->zFar();
	for (unsigned int i = 0; i < DEPTH_BUFFER_SIZE; i++)
	{
		mpDepthBuffer[i] = zFar;
	}

	if (mPBOSupported)
	{
		srt_glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
		srt_glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, COLOR_BUFFER_SIZE, 0, GL_STREAM_DRAW_ARB);
		std::unique_ptr<unsigned char[]> pColorBuffer((unsigned char*) srt_glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB));
		if (pColorBuffer != nullptr)
		{
			auto start = Time::Now();
			TraceRays(pColorBuffer);
			auto end = Time::Now();
			// DEBUG:
			std::cout << "Ray tracing took " << (end - start) << " seconds" << std::endl;
		}
		srt_glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);
		pColorBuffer.release();

		srt_glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
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
		srt_glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, mPBOId);
	}
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SimpleRayTracerApp::SCREEN_WIDTH, SimpleRayTracerApp::SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, (void*)mpTextureData.get());

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
		srt_glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
	}
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::ResetRayMetadata(RayMetadata& rRayMetadata, const Vector3F& rRayOrigin, const Vector3F& rRayDirection)
{
	rRayMetadata.start = rRayOrigin;
	rRayMetadata.direction = rRayDirection;
	rRayMetadata.isReflection = false;
	rRayMetadata.isRefraction = false;
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::SetRayMetadata(RayMetadata& rRayMetadata, const Vector3F& hitPoint) const
{
	if (rRayMetadata.next != 0)
	{
		delete rRayMetadata.next;
	}
	rRayMetadata.next = 0;
	rRayMetadata.end = hitPoint;
}

//////////////////////////////////////////////////////////////////////////
void RayTracer::TraceRays(std::unique_ptr<unsigned char[]>& colorBuffer)
{
	auto numSteps = SimpleRayTracerApp::SCREEN_WIDTH * SimpleRayTracerApp::SCREEN_HEIGHT;
	for (unsigned int y = 0, colorBufferIndex = 0, depthBufferIndex = 0, rayMetadataIndex = 0, step = 0; y < SimpleRayTracerApp::SCREEN_HEIGHT; y++)
	{
		for (unsigned int x = 0; x < SimpleRayTracerApp::SCREEN_WIDTH; x++, colorBufferIndex += 4, depthBufferIndex++, rayMetadataIndex++, step++)
		{
			Ray rRay = mScene->GetCamera()->GetRayFromScreenCoordinates(x, y);
			ResetRayMetadata(mpRaysMetadata[rayMetadataIndex], rRay.origin, rRay.direction);
			ColorRGBA color = TraceRay(rRay, mpRaysMetadata[rayMetadataIndex], &mpDepthBuffer[depthBufferIndex], 0);
			srt_setColor(colorBuffer, colorBufferIndex, color);
		}
		std::fprintf(stdout, "\rTracing rays (%.3f%%)", (step / (double)numSteps) * 100);
	}
	std::fprintf(stdout, "\n");
}

//////////////////////////////////////////////////////////////////////////
ColorRGBA RayTracer::TraceRay(const Ray& rRay, RayMetadata& rRayMetadata, float* pCurrentDepth, unsigned int iteration, std::shared_ptr<SceneObject> sceneObjectToIgnore) const
{
	ColorRGBA finalColor = SimpleRayTracerApp::CLEAR_COLOR;

	if (iteration > MAX_ITERATIONS)
	{
		return finalColor;
	}

	for (unsigned int i = 0; i < mScene->NumberOfSceneObjects(); i++)
	{
		if (auto sceneObject = mScene->GetSceneObject(i).lock())
		{
			if (sceneObjectToIgnore != 0 && sceneObject == sceneObjectToIgnore)
			{
				continue;
			}

			RayHit hit;
			if (sceneObject->Intersect(rRay, hit))
			{
				float depth = rRay.origin.Distance(hit.point);

				if (depth >= *pCurrentDepth)
				{
					continue;
				}

				*pCurrentDepth = depth;

				SetRayMetadata(rRayMetadata, hit.point);

				ColorRGBA currentColor = Shade(sceneObject, rRay, hit, rRayMetadata, iteration);

				if (sceneObject->material.transparent)
				{
					finalColor = currentColor.Blend(finalColor);
				}
				else
				{
					finalColor = currentColor;
				}
			}
		}
	}

	return finalColor;
}

//////////////////////////////////////////////////////////////////////////
ColorRGBA RayTracer::Shade(std::shared_ptr<SceneObject>& sceneObject, const Ray &rRay, const RayHit& rHit, RayMetadata& rRayMetadata, unsigned int iteration) const
{
	Material& rMaterial = sceneObject->material;
	const std::unique_ptr<Camera>& camera = mScene->GetCamera();

	ColorRGBA color = SimpleRayTracerApp::GLOBAL_AMBIENT_LIGHT * rMaterial.ambientColor;

	Vector3F viewerDirection = (rRay.origin - rHit.point).Normalized();
	const Vector3F& rNormal = rHit.normal;

	for (unsigned int j = 0; j < mScene->NumberOfLights(); j++)
	{
		const std::unique_ptr<Light>& light = mScene->GetLight(j);

		float distanceToLight = -1;
		Vector3F lightDirection;
		if (srt_is(light, DirectionalLight))
		{
			lightDirection = srt_cast(light, DirectionalLight)->direction;
		}
		else if (srt_is(light, PointLight))
		{
			Vector3F lightPosition = srt_cast(light, PointLight)->position;
			lightDirection = (lightPosition - rHit.point).Normalized();
			distanceToLight = lightPosition.Distance(rHit.point);
		}
		else 
		{
			throw std::runtime_error("unimplemented light type");
		}

		Ray shadowRay(rHit.point, lightDirection);
		if (IsLightBlocked(shadowRay, distanceToLight, sceneObject))
		{
			continue;
		}

		ColorRGBA diffuseColor = rMaterial.diffuseColor;
		if (rMaterial.texture != 0)
		{
			diffuseColor *= rMaterial.texture->Sample(rHit.uv);
		}

		ColorRGBA colorContribution = BlinnPhong(diffuseColor, rMaterial.specularColor, rMaterial.shininess, *light, lightDirection, viewerDirection, rNormal);

		if (srt_is(light, PointLight))
		{
			float distanceAttenuation = 1.0f / srt_max(srt_cast(light, PointLight)->attenuation * (distanceToLight * distanceToLight), 1);
			colorContribution *= distanceAttenuation;
		}

		color += colorContribution;
	}

	if (rMaterial.reflection > 0)
	{
		Vector3F reflectionDirection = (-viewerDirection).Reflection(rNormal).Normalized();
		Ray reflectionRay(rHit.point, reflectionDirection);
		float newDepth = camera->zFar();
		RayMetadata* pReflectionRayMetadata = new RayMetadata();
		pReflectionRayMetadata->start = reflectionRay.origin;
		pReflectionRayMetadata->direction = reflectionDirection;
		pReflectionRayMetadata->isReflection = true;
		rRayMetadata.next = pReflectionRayMetadata;
		color += rMaterial.reflection * TraceRay(reflectionRay, *pReflectionRayMetadata, &newDepth, iteration + 1, sceneObject);
	}
	else if (rMaterial.refraction > 0)
	{
		Vector3F rVt = viewerDirection.Dot(rNormal) * rNormal - viewerDirection;
		float sinI = rVt.Length();
		float sinT = rMaterial.refraction * sinI;
		float cosT = sqrt(1.0f - (sinT * sinT));
		Vector3F rT = (1.0f / rVt.Length()) * rVt;
		Vector3F rRefractionDirection = sinT * rT + cosT * (-rNormal);

		Ray refractionRay(rHit.point, rRefractionDirection);
		float newDepth = camera->zFar();
		RayMetadata* pRefractionRayMetadata = new RayMetadata();
		pRefractionRayMetadata->start = refractionRay.origin;
		pRefractionRayMetadata->direction = rRefractionDirection;
		pRefractionRayMetadata->isRefraction = true;
		rRayMetadata.next = pRefractionRayMetadata;
		color = color.Blend(TraceRay(refractionRay, *pRefractionRayMetadata, &newDepth, iteration + 1, sceneObject));
	}

	srt_clampColor(color, 0, 1);

	return color;
}

//////////////////////////////////////////////////////////////////////////
bool RayTracer::IsLightBlocked(const Ray& rShadowRay, float distanceToLight, std::shared_ptr<SceneObject> origin) const
{
	for (unsigned int i = 0; i < mScene->NumberOfSceneObjects(); i++)
	{
		if (auto sceneObject = mScene->GetSceneObject(i).lock())
		{
			if (sceneObject == origin)
			{
				continue;
			}

			RayHit hit;
			if (sceneObject->Intersect(rShadowRay, hit))
			{
				if (distanceToLight == -1)
				{
					return true;
				}
				else if (distanceToLight > hit.point.Distance(rShadowRay.origin))
				{
					return true;
				}
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


