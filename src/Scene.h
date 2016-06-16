#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include <memory>

#include "Camera.h"
#include "Light.h"
#include "SceneObject.h"

class Scene
{
public:
	Scene() :
	  mCamera(nullptr)
	{
	}

	~Scene()
	{
		mLights.clear();
		mSceneObjects.clear();
		mCamera = nullptr;
	}

	inline void SetCamera(std::unique_ptr<Camera>& camera)
	{
		mCamera = std::move(camera);
	}

	inline std::unique_ptr<Camera>& GetCamera()
	{
		return mCamera;
	}

	inline const std::unique_ptr<Camera>& GetCamera() const
	{
		return mCamera;
	}

	inline void AddLight(std::unique_ptr<Light>&& pLight)
	{
		mLights.emplace_back(std::move(pLight));
	}

	inline void AddSceneObject(std::shared_ptr<SceneObject>& pSceneObject)
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

	inline const std::unique_ptr<Light>& GetLight(unsigned int i) const
	{
		return mLights[i];
	}

	inline std::weak_ptr<SceneObject> GetSceneObject(unsigned int i) const
	{
		return mSceneObjects[i];
	}

	void Update()
	{
		mCamera->Update();

		for (unsigned int i = 0; i < mSceneObjects.size(); i++)
		{
			mSceneObjects[i]->Update();
		}
	}

private:
	std::unique_ptr<Camera> mCamera;
	std::vector<std::unique_ptr<Light> > mLights;
	std::vector<std::shared_ptr<SceneObject> > mSceneObjects;

};

#endif