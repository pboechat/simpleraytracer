#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

#include "Ray.h"
#include "RayHit.h"
#include "Vector3F.h"
#include "Material.h"

#define _USE_MATH_DEFINES

#include <math.h>

struct SceneObject
{
	Material material;

	virtual bool Intersect(const Ray& rRay, RayHit& rHit) const = 0;

};

#endif