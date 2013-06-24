#ifndef OPENGLRENDERER_H_
#define OPENGLRENDERER_H_

#include "Camera.h"
#include "Scene.h"

class OpenGLRenderer
{
public:
	OpenGLRenderer(const Camera* pCamera, const Scene* pScene);

	void Render();

private:
	const Camera* mpCamera;
	const Scene* mpScene;

};

#endif