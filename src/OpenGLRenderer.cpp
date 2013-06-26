#include "OpenGLRenderer.h"
#include "Vector2F.h"
#include "Vector3F.h"
#include "Light.h"

#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#define cast(x, y) static_cast<y*>(x)
#define is(x, y) static_cast<y*>(x) != 0

OpenGLRenderer::OpenGLRenderer(const Camera* pCamera, const Scene* pScene) :
	mpCamera(pCamera),
	mpScene(pScene)
{
}

void OpenGLRenderer::Render()
{
	for (unsigned int i = 0; i < mpScene->NumberOfLights(); i++)
	{
		glEnable(GL_LIGHT0 + i);
		Light* pLight = mpScene->GetLight(i);

		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, pLight->diffuseColor[0]);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, pLight->specularColor[0]);
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
	for (unsigned int i = 0; i < pMesh->indices.size(); i += 3)
	{
		unsigned int i1 = pMesh->indices[i];
		unsigned int i2 = pMesh->indices[i + 1];
		unsigned int i3 = pMesh->indices[i + 2];

		Vector3F& rV1 = pMesh->vertices[i1];
		Vector3F& rV2 = pMesh->vertices[i2];
		Vector3F& rV3 = pMesh->vertices[i3];

		Vector3F& rN1 = pMesh->normals[i1];
		Vector3F& rN2 = pMesh->normals[i2];
		Vector3F& rN3 = pMesh->normals[i3];

		Vector2F& rUV1 = pMesh->uvs[i1];
		Vector2F& rUV2 = pMesh->uvs[i2];
		Vector2F& rUV3 = pMesh->uvs[i3];

		bool hasTexture = pMesh->material.texture != 0;
		if (hasTexture)
		{
			unsigned int textureId;
			std::map<Mesh*, unsigned int>::iterator it = mTextureIds.find(pMesh);
			if (it == mTextureIds.end())
			{
				textureId = AllocateTextureForMesh(pMesh);
			}
			else
			{
				textureId = it->second;
			}

			glBindTexture(GL_TEXTURE_2D, textureId);
		}

		glBegin(GL_TRIANGLES);
			if (hasTexture)
			{
				glTexCoord2f(rUV1.x, rUV1.y);
			}
			glNormal3f(rN1.x, rN1.y, rN1.z);
			glVertex3f(rV1.x, rV1.y, rV1.z);

			if (hasTexture)
			{
				glTexCoord2f(rUV2.x, rUV2.y);
			}
			glNormal3f(rN2.x, rN2.y, rN2.z);
			glVertex3f(rV2.x, rV2.y, rV2.z);

			if (hasTexture)
			{
				glTexCoord2f(rUV3.x, rUV3.y);
			}
			glNormal3f(rN3.x, rN3.y, rN3.z);
			glVertex3f(rV3.x, rV3.y, rV3.z);
		glEnd();
	}
}

void OpenGLRenderer::RenderSphere(Sphere* pSphere)
{
}

unsigned int OpenGLRenderer::AllocateTextureForMesh(Mesh* pMesh)
{
	unsigned int textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, pMesh->material.texture->width, pMesh->material.texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pMesh->material.texture->data);

	mTextureIds.insert(std::make_pair(pMesh, textureId));

	return textureId;
}
