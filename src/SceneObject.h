#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

#include "Ray.h"
#include "RayHit.h"
#include "Transform.h"
#include "Material.h"
#include "Vector3F.h"
#include "Vector4F.h"
#include "Matrix3x3F.h"
#include "Matrix4x4F.h"

#include <cmath>
#include <vector>
#include <memory>

struct SceneObject
{
	std::weak_ptr<SceneObject> parent;
	std::vector<std::weak_ptr<SceneObject> > children;
	Material material;
	Transform localTransform;

	virtual ~SceneObject()
	{
	}

	inline Matrix4x4F model() const
	{
		return mWorldTransform.ToMatrix4x4F();
	}

	virtual bool Intersect(const Ray& rRay, RayHit& rHit) const
	{
		return false;
	}

	virtual void Update()
	{
		mWorldTransform = localTransform;

		if (auto parentPtr = parent.lock())
		{
			mWorldTransform = parentPtr->mWorldTransform * mWorldTransform;
		}

		for (unsigned int i = 0; i < children.size(); i++)
		{
			if (auto childPtr = children[i].lock())
			{
				childPtr->Update();
			}
		}

		OnUpdate();
	}

protected:
	Transform mWorldTransform;

	SceneObject()
	{
	}

	virtual void OnUpdate()
	{
	}

};

#endif