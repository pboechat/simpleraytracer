#ifndef SCENE_H_
#define SCENE_H_

#include "Light.h"
#include "SceneObject.h"

#include <vector>

class Scene
{
public:
	Scene()
	{
	}

	~Scene()
	{
		Clear();
	}

	inline void AddLight(Light* pLight)
	{
		mLights.push_back(pLight);
	}

	inline void AddSceneObject(SceneObject* pSceneObject)
	{
		mSceneObjects.push_back(pSceneObject);
	}

	inline unsigned int NumberOfLights() const
	{
		return mLights.size();
	}

	inline unsigned int NumberOfSceneObjects() const
	{
		return mSceneObjects.size();
	}

	inline Light* GetLight(unsigned int i) const
	{
		return mLights[i];
	}

	inline SceneObject* GetSceneObject(unsigned int i) const
	{
		return mSceneObjects[i];
	}

	void Clear()
	{
		for (unsigned int i = 0; i < mLights.size(); i++)
		{
			delete mLights[i];
		}
		mLights.clear();

		for (unsigned int i = 0; i < mSceneObjects.size(); i++)
		{
			delete mSceneObjects[i];
		}
		mSceneObjects.clear();
	}

	void Update()
	{
		for (unsigned int i = 0; i < mSceneObjects.size(); i++)
		{
			mSceneObjects[i]->Update();
		}
	}

private:
	std::vector<Light*> mLights;
	std::vector<SceneObject*> mSceneObjects;

};

#endif