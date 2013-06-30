#ifndef RENDERER_H_
#define RENDERER_H_

#include "Scene.h"

class Renderer
{
public:
	virtual ~Renderer() {}

	inline void SetScene(const Scene* pScene)
	{
		mpScene = pScene;
		OnSetScene();
	}

	virtual void Start()
	{
	}

	virtual void Render() = 0;

protected:
	const Scene* mpScene;

	Renderer() : 
		mpScene(0) 
	{
	}

	virtual void OnSetScene()
	{
	}

};

#endif