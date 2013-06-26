#ifndef OPENGLRENDERER_H_
#define OPENGLRENDERER_H_

#include "Camera.h"
#include "Scene.h"
#include "Sphere.h"
#include "Mesh.h"

#include <map>

class OpenGLRenderer
{
public:
	OpenGLRenderer(const Camera* pCamera, const Scene* pScene);
	~OpenGLRenderer();

	void Render();
	
private:
	const Camera* mpCamera;
	const Scene* mpScene;
	std::map<Mesh*, unsigned int> mTextureIds;

	void RenderMesh(Mesh* pMesh);
	void RenderSphere(Sphere* pSphere);
	unsigned int AllocateTextureForMesh(Mesh* pMesh);

};

#endif