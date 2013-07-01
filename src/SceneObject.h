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

struct SceneObject
{
	SceneObject* parent;
	std::vector<SceneObject*> children;
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

		if (parent != 0)
		{
			mWorldTransform = parent->mWorldTransform * mWorldTransform;
		}

		for (unsigned int i = 0; i < children.size(); i++)
		{
			children[i]->Update();
		}

		OnUpdate();
	}

protected:
	Transform mWorldTransform;

	SceneObject() :
		parent(0)
	{
	}

	virtual void OnUpdate()
	{
	}

};

#endif