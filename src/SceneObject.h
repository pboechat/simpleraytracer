#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

#include "Ray.h"
#include "RayHit.h"
#include "Transform.h"
#include "Material.h"

#include <cmath>
#include <vector>

struct SceneObject
{
	SceneObject* parent;
	std::vector<SceneObject*> children;
	Material material;
	Transform localTransform;

	virtual bool Intersect(const Ray& rRay, RayHit& rHit) const = 0;

	inline void Update()
	{
		mWorldTransform = localTransform;

		if (parent != 0)
		{
			mWorldTransform *= parent->mWorldTransform;
		}

		for (unsigned int i = 0; i < children.size(); i++)
		{
			children[i]->Update();
		}
	}

protected:
	Transform mWorldTransform;

	SceneObject() :
		parent(0)
	{
	}

};

#endif