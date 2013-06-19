#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

#include "Ray.h"
#include "RayHit.h"
#include "Transform.h"
#include "Material.h"

#define _USE_MATH_DEFINES

#include <math.h>

struct SceneObject
{
	SceneObject* parent;
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
	}

protected:
	Transform mWorldTransform;

	SceneObject() :
		parent(0)
	{
	}

};

#endif