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

	inline Matrix4x4F model() const
	{
		Matrix4x4F model;
		ToMatrix4x4(model, mWorldTransform.rotation);

		model[0][3] = mWorldTransform.position.x();
		model[1][3] = mWorldTransform.position.y();
		model[2][3] = mWorldTransform.position.z();

		Matrix4x4F scale;
		ToMatrix4x4(scale, mWorldTransform.scale);

		return model * scale;
	}

	virtual bool Intersect(const Ray& rRay, RayHit& rHit) const = 0;

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
	}

protected:
	Transform mWorldTransform;

	SceneObject() :
		parent(0)
	{
	}

};

#endif