#include "OpenGLRenderer.h"
#include "Camera.h"
#include "Vector2F.h"
#include "Vector3F.h"
#include "Vector4F.h"
#include "Matrix3x3F.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Application.h"

#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include "glext.h"
#include <cmath>
#include <exception>
#include <vector>

#include "Common.h"

#define SPHERE_MESH_SLICES 60

//////////////////////////////////////////////////////////////////////////
OpenGLRenderer::OpenGLRenderer() :
	mDebugRayEnabled(false)
{
}

//////////////////////////////////////////////////////////////////////////
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

	const Camera* pCamera = mpScene->GetCamera();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(pCamera->projection().Transpose()[0]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(Application::CLEAR_COLOR.r(), Application::CLEAR_COLOR.g(), Application::CLEAR_COLOR.b(), Application::CLEAR_COLOR.a());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &Application::GLOBAL_AMBIENT_LIGHT[0]);

	for (unsigned int i = 0; i < mpScene->NumberOfLights(); i++)
	{
		glEnable(GL_LIGHT0 + i);
		Light* pLight = mpScene->GetLight(i);

		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, &(pLight->diffuseColor * pLight->intensity)[0]);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, &(pLight->specularColor * pLight->intensity)[0]);

		if (is(pLight, DirectionalLight))
		{
			Vector4F lightDirection = pCamera->inverseRotation() * cast(pLight, DirectionalLight)->direction.ToVector4F();
			lightDirection.w() = 0;

			glLightfv(GL_LIGHT0 + i, GL_POSITION, &lightDirection[0]);
		}
		else if (is(pLight, PointLight))
		{
			Vector4F lightPosition = pCamera->view() * cast(pLight, PointLight)->position.ToVector4F();
			lightPosition.w() = 1;

			glLightfv(GL_LIGHT0 + i, GL_POSITION, &lightPosition[0]);
			glLightf(GL_LIGHT0 + i, GL_CONSTANT_ATTENUATION, 0);
			glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, 0);
			glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, 1);
		}
	}

	glLoadMatrixf(pCamera->view().Transpose()[0]);

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

	if (mDebugRayEnabled)
	{
		glDisable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glLineWidth(1.5f);
		glColor4f(1, 1, 1, 1);

		const RayMetadata* pRay = &mRayToDebug;
		while (pRay != 0)
		{
			if (pRay->end.x() == -1)
			{
				break;
			}

			glBegin(GL_LINES);
			glVertex3fv(&pRay->start[0]);
			glVertex3fv(&pRay->end[0]);
			glEnd();

			pRay = pRay->next;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void OpenGLRenderer::RenderMesh(Mesh* pMesh)
{
	SetUpMaterial(pMesh);
	RenderTriangles(pMesh->model(), pMesh->indices, pMesh->vertices, pMesh->normals, pMesh->uvs);
}

//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
unsigned int OpenGLRenderer::AllocateTextureForSceneObject(SceneObject* pSceneObject)
{
	unsigned int textureId;

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pSceneObject->material.texture->width, pSceneObject->material.texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pSceneObject->material.texture->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);

	mTextureIds.insert(std::make_pair(pSceneObject, textureId));

	return textureId;
}

//////////////////////////////////////////////////////////////////////////
Mesh* OpenGLRenderer::CreateMeshForSphere(Sphere* pSphere)
{
	Mesh* pMesh = new Mesh();

	const unsigned int vertexQuantity = (SPHERE_MESH_SLICES - 2) * (SPHERE_MESH_SLICES + 1) + 2;
	const unsigned int triangleQuantity = 2 * (SPHERE_MESH_SLICES - 2) * SPHERE_MESH_SLICES;

	const float invRS = 1.0f / static_cast<float>(SPHERE_MESH_SLICES);
	const float zFactor = 2.0f / static_cast<float>(SPHERE_MESH_SLICES - 1);

	pMesh->vertices = std::vector<Vector3F>(vertexQuantity);
	pMesh->normals = std::vector<Vector3F>(vertexQuantity);
	pMesh->uvs = std::vector<Vector2F>(vertexQuantity);
	pMesh->indices = std::vector<unsigned int>(3 * triangleQuantity);

	float* pSin = new float[SPHERE_MESH_SLICES + 1];
	float* pCos = new float[SPHERE_MESH_SLICES + 1];
	for (unsigned int iR = 0; iR < SPHERE_MESH_SLICES; iR++)
	{
		float angle = 2 * PI * invRS * iR;
		pCos[iR] = cos(angle);
		pSin[iR] = sin(angle);
	}

	pSin[SPHERE_MESH_SLICES] = pSin[0];
	pCos[SPHERE_MESH_SLICES] = pCos[0];

	unsigned int i = 0;
	for (unsigned int iZ = 1; iZ < SPHERE_MESH_SLICES - 1; iZ++)
	{
		const float zFraction = -1 + zFactor * iZ;  // in (-1,1)
		const float z = pSphere->radius * zFraction;

		// compute center of slice
		Vector3F sliceCenter(0, 0, z);

		// compute radius of slice
		float sliceRadius = sqrt(abs(pSphere->radius * pSphere->radius - z * z));

		Vector3F normal;
		unsigned int iSave = i;
		for (unsigned int iR = 0; iR < SPHERE_MESH_SLICES; iR++)
		{
			float radialFraction = iR * invRS;  // in [0,1)
			Vector3F vertex = sliceCenter + sliceRadius * Vector3F(pCos[iR], pSin[iR], 0);
			pMesh->vertices[i] = vertex;
			pMesh->normals[i] = (-vertex).Normalized();
			pMesh->uvs[i] = Vector2F(radialFraction, 0.5f * (zFraction + 1));
			i++;
		}

		pMesh->vertices[i] = pMesh->vertices[iSave];
		pMesh->normals[i] = pMesh->normals[iSave];
		pMesh->uvs[i] = Vector2F(1, 0.5f * (zFraction + 1));

		i++;
	}

	// south pole
	pMesh->vertices[i] = Vector3F(0, 0, -pSphere->radius);
	pMesh->normals[i] = Vector3F(0, 0, 1);
	pMesh->uvs[i] = Vector2F(0.5f, 0.5f);

	i++;

	// north pole
	pMesh->vertices[i] = Vector3F(0, 0, pSphere->radius);
	pMesh->normals[i] = Vector3F(0, 0, -1);
	pMesh->uvs[i] = Vector2F(0.5f, 1);

	i++;
	unsigned int rsc = SPHERE_MESH_SLICES;
	unsigned int vq = vertexQuantity;
	unsigned int zsc = SPHERE_MESH_SLICES;

	// generate connectivity
	unsigned int iZStart = 0;
	unsigned int c = 0;
	for (unsigned int iZ = 0; iZ < SPHERE_MESH_SLICES - 3; iZ++)
	{
		unsigned int i0 = iZStart;
		unsigned int i1 = i0 + 1;
		iZStart += rsc + 1;
		unsigned int i2 = iZStart;
		unsigned int i3 = i2 + 1;
		for (unsigned int i = 0; i < SPHERE_MESH_SLICES; i++)
		{
			pMesh->indices[c] = i0++;
			pMesh->indices[c + 1] = i2;
			pMesh->indices[c + 2] = i1;
			pMesh->indices[c + 3] = i1++;
			pMesh->indices[c + 4] = i2++;
			pMesh->indices[c + 5] = i3++;
			c += 6;
		}
	}

	// south pole triangles
	unsigned int vQm2 = vq - 2;
	for (unsigned int i = 0; i < rsc; i++)
	{
		pMesh->indices[c] = i;
		pMesh->indices[c + 1] = i + 1;
		pMesh->indices[c + 2] = vQm2;
		c += 3;
	}

	// north pole triangles
	unsigned int vQm1 = vq - 1;
	unsigned int offset = (zsc - 3) * (rsc + 1);
	for (unsigned int i = 0; i < rsc; i++)
	{
		pMesh->indices[c] = i + offset;
		pMesh->indices[c + 1] = vQm1;
		pMesh->indices[c + 2] = i+1+offset;
		c += 3;
	}

	delete[] pCos;
	delete[] pSin;

	return pMesh;
}

//////////////////////////////////////////////////////////////////////////
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
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT, &pSceneObject->material.ambientColor[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &pSceneObject->material.diffuseColor[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &pSceneObject->material.specularColor[0]);
	glMaterialf(GL_FRONT, GL_SHININESS, pSceneObject->material.shininess);
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

