#include "OpenGLRenderer.h"
#include "Vector2F.h"
#include "Vector3F.h"
#include "Vector4F.h"
#include "Matrix3x3F.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#define SPHERE_MESH_SLICES 60

#include <cmath>
#include <vector>

#define cast(x, y) dynamic_cast<y*>(x)
#define is(x, y) dynamic_cast<y*>(x) != 0

OpenGLRenderer::OpenGLRenderer(const Camera* pCamera, const Scene* pScene) :
	mpCamera(pCamera),
	mpScene(pScene)
{
}

OpenGLRenderer::~OpenGLRenderer()
{
	std::map<SceneObject*, unsigned int>::iterator it;
	for (it = mTextureIds.begin(); it != mTextureIds.end(); it++)
	{
		glDeleteTextures(1, &it->second);
	}
	mTextureIds.clear();

	std::map<Sphere*, Mesh*>::iterator it2;
	for (it2 = mSphereMeshes.begin(); it2 != mSphereMeshes.end(); it2++)
	{
		delete it2->second;
	}
	mSphereMeshes.clear();
}

void OpenGLRenderer::Render()
{
	for (unsigned int i = 0; i < mpScene->NumberOfLights(); i++)
	{
		glEnable(GL_LIGHT0 + i);
		Light* pLight = mpScene->GetLight(i);

		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, &(pLight->diffuseColor * pLight->intensity)[0]);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, &(pLight->specularColor * pLight->intensity)[0]);

		if (is(pLight, DirectionalLight))
		{
			glLightfv(GL_LIGHT0 + i, GL_POSITION, &cast(pLight, DirectionalLight)->direction[0]);
		}
		else if (is(pLight, PointLight))
		{
			Vector4F lightPosition;
			ToVector4F(lightPosition, cast(pLight, PointLight)->position);
			glLightfv(GL_LIGHT0 + i, GL_POSITION, &(mpCamera->GetViewMatrix() * lightPosition)[0]);
			glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 0);
			glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0);
			glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 1);
		}
	}

	for (unsigned int i = 0; i < mpScene->NumberOfSceneObjects(); i++)
	{
		SceneObject* pSceneObject = mpScene->GetSceneObject(i);

		if (is(pSceneObject, Mesh))
		{
			RenderMesh(cast(pSceneObject, Mesh));
		}
		else if (is(pSceneObject, Sphere))
		{
			RenderSphere(cast(pSceneObject, Sphere));
		}
	}
}

void OpenGLRenderer::RenderMesh(Mesh* pMesh)
{
	SetUpMaterial(pMesh);
	RenderTriangles(pMesh->model(), pMesh->indices, pMesh->vertices, pMesh->normals, pMesh->uvs);
}

void OpenGLRenderer::RenderSphere(Sphere* pSphere)
{
	Mesh* pMesh;
	std::map<Sphere*, Mesh*>::iterator it = mSphereMeshes.find(pSphere);
	if (it != mSphereMeshes.end())
	{
		pMesh = it->second;
	}
	else
	{
		pMesh = CreateMeshForSphere(pSphere);
		mSphereMeshes.insert(std::make_pair(pSphere, pMesh));
	}
	
	SetUpMaterial(pSphere);
	RenderTriangles(pSphere->model(), pMesh->indices, pMesh->vertices, pMesh->normals, pMesh->uvs);
}

unsigned int OpenGLRenderer::AllocateTextureForSceneObject(SceneObject* pSceneObject)
{
	unsigned int textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, pSceneObject->material.texture->width, pSceneObject->material.texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pSceneObject->material.texture->data);

	mTextureIds.insert(std::make_pair(pSceneObject, textureId));

	return textureId;
}

Mesh* OpenGLRenderer::CreateMeshForSphere(Sphere* pSphere)
{
	Mesh* pMesh = new Mesh();

	float u_step = 2 * PI / (SPHERE_MESH_SLICES - 1);
	float v_step = PI / (SPHERE_MESH_SLICES - 1);
	float u = 0.0f;
	for (unsigned int i = 0; i < SPHERE_MESH_SLICES; i++)
	{
		float cos_u = cos(u);
		float sin_u = sin(u);

		float v = 0.0f;
		for (unsigned int j = 0; j < SPHERE_MESH_SLICES; j++)
		{
			float cos_v = cos(v);
			float sin_v = sin(v);

			float nx = sin_v * cos_u;
			float ny = -cos_v;
			float nz = -sin_v * sin_u;

			float n = sqrt(nx * nx + ny * ny + nz * nz);

			if (n < 0.99f || n > 1.01f)
			{
				nx = nx / n;
				ny = ny / n;
				nz = nz / n;
			}

			float tx = nz;
			float ty = 0;
			float tz = -nx;

			float a = sqrt(tx * tx + ty * ty + tz * tz);

			if (a > 0.001f) 
			{
				tx = tx / a;
				ty = ty / a;
				tz = tz / a;
			}

			float x = pSphere->radius * nx;
			float y = pSphere->radius * ny;
			float z = pSphere->radius * nz;

			pMesh->vertices.push_back(Vector3F(x, y, z));
			pMesh->normals.push_back(Vector3F(nx, ny, nz));
			pMesh->uvs.push_back(Vector2F(u / (2 * PI), v / PI));

			v += v_step;
		}
		u += u_step;
	}

	for (unsigned int i = 0; i < SPHERE_MESH_SLICES - 1; i++)
	{
		for (unsigned j = 0; j < SPHERE_MESH_SLICES - 1; j++)
		{
			unsigned int p = i * SPHERE_MESH_SLICES + j;
			pMesh->indices.push_back(p);
			pMesh->indices.push_back(p + SPHERE_MESH_SLICES);
			pMesh->indices.push_back(p + SPHERE_MESH_SLICES + 1);
			pMesh->indices.push_back(p);
			pMesh->indices.push_back(p + SPHERE_MESH_SLICES + 1);
			pMesh->indices.push_back(p + 1);
		}
	}

	return pMesh;
}

void OpenGLRenderer::SetUpMaterial(SceneObject* pSceneObject)
{
	if (pSceneObject->material.texture != 0)
	{
		unsigned int textureId;
		std::map<SceneObject*, unsigned int>::iterator it = mTextureIds.find(pSceneObject);
		if (it == mTextureIds.end())
		{
			textureId = AllocateTextureForSceneObject(pSceneObject);
		}
		else
		{
			textureId = it->second;
		}

		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT, &pSceneObject->material.ambientColor[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &pSceneObject->material.diffuseColor[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &pSceneObject->material.specularColor[0]);
	glMaterialf(GL_FRONT, GL_SHININESS, pSceneObject->material.shininess);
}

void OpenGLRenderer::RenderTriangles(const Matrix4F& model, const std::vector<unsigned int>& indices, const std::vector<Vector3F>& vertices, const std::vector<Vector3F>& normals, const std::vector<Vector2F>& uvs)
{
	glPushMatrix();
	const Matrix4F& rModelView = mpCamera->GetViewMatrix() * model;
	glLoadMatrixf(rModelView.Transpose()[0]);

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
