#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

#include "Ray.h"
#include "RayHit.h"
#include "Vector3F.h"
#include "Material.h"

#define _USE_MATH_DEFINES

#include <math.h>

class SceneObject
{
public:
	Material material;

	virtual bool Intersect(const Ray& rRay, RayHit& rHit) = 0;

};

#endif