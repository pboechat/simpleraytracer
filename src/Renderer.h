#ifndef RENDERER_H_
#define RENDERER_H_

#include "Scene.h"

class Renderer
{
public:
	virtual ~Renderer() {}

	inline void SetScene(std::shared_ptr<Scene>& scene)
	{
		mScene = scene;
		OnSetScene();
	}

	virtual void Start()
	{
	}

	virtual void Render() = 0;

protected:
	std::shared_ptr<Scene> mScene;

	Renderer() : 
		mScene(nullptr) 
	{
	}

	virtual void OnSetScene()
	{
	}

};

#endif