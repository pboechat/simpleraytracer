#include <cmath>
#include <stdexcept>
#include <vector>
#include <cassert>
#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include "glext.h"

#include "Common.h"
#include "OpenGLRenderer.h"
#include "Camera.h"
#include "Vector2F.h"
#include "Vector3F.h"
#include "Vector4F.h"
#include "Matrix3x3F.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SimpleRayTracerApp.h"

#define SPHERE_MESH_SLICES 100

//////////////////////////////////////////////////////////////////////////
const float OpenGLRenderer::MISSED_RAY_LENGTH = 100000.0f;

//////////////////////////////////////////////////////////////////////////
OpenGLRenderer::OpenGLRenderer() :
	mDebugRayEnabled(false),
	mpRayToDebug(nullptr)
{
}

//////////////////////////////////////////////////////////////////////////
OpenGLRenderer::~OpenGLRenderer()
{
	for (auto it = mTextureIds.begin(); it != mTextureIds.end(); it++)
	{
		glDeleteTextures(1, &it->second);
	}
	mTextureIds.clear();
	mSphereMeshes.clear();
}

//////////////////////////////////////////////////////////////////////////
void OpenGLRenderer::Render()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	const std::unique_ptr<Camera>& camera = mScene->GetCamera();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(camera->projection().Transpose()[0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(SimpleRayTracerApp::CLEAR_COLOR.r(), SimpleRayTracerApp::CLEAR_COLOR.g(), SimpleRayTracerApp::CLEAR_COLOR.b(), SimpleRayTracerApp::CLEAR_COLOR.a());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &SimpleRayTracerApp::GLOBAL_AMBIENT_LIGHT[0]);

	for (unsigned int i = 0; i < mScene->NumberOfLights(); i++)
	{
		glEnable(GL_LIGHT0 + i);
		const std::unique_ptr<Light>& light = mScene->GetLight(i);

		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, &(light->diffuseColor * light->intensity)[0]);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, &(light->specularColor * light->intensity)[0]);

		if (srt_is(light, DirectionalLight))
		{
			Vector4F lightDirection(camera->inverseRotation() * -srt_ptr_cast(light, DirectionalLight)->direction, 0 /* NOTE: indicates directional light for OpenGL */);
			glLightfv(GL_LIGHT0 + i, GL_POSITION, &lightDirection[0]);
		}
		else if (srt_is(light, PointLight))
		{
			Vector4F lightPosition((camera->view() * Vector4F(srt_ptr_cast(light, PointLight)->position, 1)).xyz(), 1 /* NOTE: indicates point light for OpenGL */);
			glLightfv(GL_LIGHT0 + i, GL_POSITION, &lightPosition[0]);
			glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 0);
			glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0);
			glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 1);
		}
	}

	glLoadMatrixf(camera->view().Transpose()[0]);

	for (unsigned int i = 0; i < mScene->NumberOfSceneObjects(); i++)
	{
		if (auto sceneObject = mScene->GetSceneObject(i).lock())
		{
			SetUpMaterial(i, sceneObject);
			if (srt_is(sceneObject, Mesh))
			{
				RenderMesh(i, std::dynamic_pointer_cast<Mesh>(sceneObject));
			}
			else if (srt_is(sceneObject, Sphere))
			{
				RenderSphere(i, std::dynamic_pointer_cast<Sphere>(sceneObject));
			}
		}
	}

	if (mDebugRayEnabled)
	{
		RenderRay();
	}
}

//////////////////////////////////////////////////////////////////////////
void OpenGLRenderer::RenderMesh(unsigned int i, std::shared_ptr<Mesh>& mesh)
{
	RenderTriangles(mesh->model(), mesh->indices, mesh->vertices, mesh->normals, mesh->uvs);
}

//////////////////////////////////////////////////////////////////////////
void OpenGLRenderer::RenderSphere(unsigned int i, std::shared_ptr<Sphere>& sphere)
{
	auto it = mSphereMeshes.find(i);
	if (it == mSphereMeshes.end())
	{
		mSphereMeshes.emplace(std::make_pair(i, CreateMeshForSphere(sphere)));
		it = mSphereMeshes.find(i);
		assert(it != mSphereMeshes.end());
	}
	RenderTriangles(sphere->model(), it->second->indices, it->second->vertices, it->second->normals, it->second->uvs);
}

//////////////////////////////////////////////////////////////////////////
void OpenGLRenderer::RenderRay()
{
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glLineWidth(1.5f);

	auto* pRay = mpRayToDebug;
	while (pRay != nullptr)
	{
		Vector3F end = pRay->end;
		bool missed = end.x() == -1;
		if (missed)
			end = pRay->start + (pRay->direction * MISSED_RAY_LENGTH);
		if (pRay->isReflection)
		{
			glColor4f(0, 1, 1, 1);
		}
		else if (pRay->isRefraction)
		{
			glColor4f(0.8f, 0.8f, 0.8f, 1);
		}
		else if (missed)
		{
			glColor4f(0.333f, 0.333f, 0.333f, 1);
		}
		else
		{
			glColor4f(1, 1, 0, 1);
		}
		glBegin(GL_LINES);
		glVertex3fv(&pRay->start[0]);
		glVertex3fv(&end[0]);
		glEnd();
		pRay = pRay->next.get();
		assert(!missed || (missed && pRay == nullptr));
	}
}

//////////////////////////////////////////////////////////////////////////
unsigned int OpenGLRenderer::AllocateTextureForSceneObject(unsigned int i, std::shared_ptr<SceneObject>& sceneObject)
{
	unsigned int textureId;

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sceneObject->material.texture->width, sceneObject->material.texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, sceneObject->material.texture->data.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

	mTextureIds.insert(std::make_pair(i, textureId));

	return textureId;
}

//////////////////////////////////////////////////////////////////////////
std::unique_ptr<Mesh> OpenGLRenderer::CreateMeshForSphere(std::shared_ptr<Sphere>& sphere)
{
	std::unique_ptr<Mesh> mesh(new Mesh());

	float uStep = 2 * srt_PI / (SPHERE_MESH_SLICES - 1);
	float vStep = srt_PI / (SPHERE_MESH_SLICES - 1);
	float u = 0.0f;
	for (unsigned int i = 0; i < SPHERE_MESH_SLICES; i++)
	{
		float cosU = cos(u);
		float sinU = sin(u);

		float v = 0.0f;
		for (unsigned int j = 0; j < SPHERE_MESH_SLICES; j++)
		{
			float cosV = cos(v);
			float sinV = sin(v);

			float nX = sinV * cosU;
			float nY = -cosV;
			float nZ = -sinV * sinU;

			float n = sqrt( nX * nX + nY * nY + nZ * nZ );

			if (n < 0.99f || n > 1.01f)
			{
				nX = nX / n;
				nY = nY / n;
				nZ = nZ / n;
			}

			float x = sphere->radius * nX;
			float y = sphere->radius * nY;
			float z = sphere->radius * nZ;

			Vector3F normal(nX, nY, nZ);

			mesh->vertices.push_back(Vector3F(x, y, z));
			mesh->normals.push_back(normal);

			mesh->uvs.push_back(Vector2F(asin(normal.x()) / srt_PI + 0.5f, asin(normal.y()) / srt_PI + 0.5f));

			v += vStep;
		}
		u += uStep;
	}

	for (unsigned int i = 0; i < SPHERE_MESH_SLICES - 1; i++)
	{
		for (unsigned j = 0; j < SPHERE_MESH_SLICES - 1; j++)
		{
			unsigned int p = i * SPHERE_MESH_SLICES + j;
			mesh->indices.push_back(p);
			mesh->indices.push_back(p + SPHERE_MESH_SLICES);
			mesh->indices.push_back(p + SPHERE_MESH_SLICES + 1);
			mesh->indices.push_back(p);
			mesh->indices.push_back(p + SPHERE_MESH_SLICES + 1);
			mesh->indices.push_back(p + 1);
		}
	}

	return mesh;
}

//////////////////////////////////////////////////////////////////////////
void OpenGLRenderer::SetUpMaterial(unsigned int i, std::shared_ptr<SceneObject>& sceneObject)
{
	if (sceneObject->material.texture != 0)
	{
		unsigned int textureId;
		auto it = mTextureIds.find(i);
		if (it == mTextureIds.end())
		{
			textureId = AllocateTextureForSceneObject(i, sceneObject);
		}
		else
		{
			textureId = it->second;
		}

		glBindTexture(GL_TEXTURE_2D, textureId);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT, &sceneObject->material.ambientColor[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &sceneObject->material.diffuseColor[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &sceneObject->material.specularColor[0]);
	glMaterialf(GL_FRONT, GL_SHININESS, sceneObject->material.shininess);
}

//////////////////////////////////////////////////////////////////////////
void OpenGLRenderer::RenderTriangles(const Matrix4F& model, const std::vector<unsigned int>& indices, const std::vector<Vector3F>& vertices, const std::vector<Vector3F>& normals, const std::vector<Vector2F>& uvs)
{
	glPushMatrix();
		glMultMatrixf(model.Transpose()[0]);

		glBegin(GL_TRIANGLES);
		for (unsigned int i = 0; i < indices.size(); i += 3)
		{
			unsigned int i1 = indices[i];
			unsigned int i2 = indices[i + 1];
			unsigned int i3 = indices[i + 2];

			const Vector3F& rV1 = vertices[i1];
			const Vector3F& rV2 = vertices[i2];
			const Vector3F& rV3 = vertices[i3];

			const Vector3F& rN1 = normals[i1];
			const Vector3F& rN2 = normals[i2];
			const Vector3F& rN3 = normals[i3];

			const Vector2F& rUV1 = uvs[i1];
			const Vector2F& rUV2 = uvs[i2];
			const Vector2F& rUV3 = uvs[i3];

			glTexCoord2f(rUV1.x(), rUV1.y());
			glNormal3f(rN1.x(), rN1.y(), rN1.z());
			glVertex3f(rV1.x(), rV1.y(), rV1.z());

			glTexCoord2f(rUV2.x(), rUV2.y());
			glNormal3f(rN2.x(), rN2.y(), rN2.z());
			glVertex3f(rV2.x(), rV2.y(), rV2.z());

			glTexCoord2f(rUV3.x(), rUV3.y());
			glNormal3f(rN3.x(), rN3.y(), rN3.z());
			glVertex3f(rV3.x(), rV3.y(), rV3.z());
		}
		glEnd();
	glPopMatrix();
}

