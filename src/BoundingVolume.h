#ifndef BOUNDINGVOLUME_H_
#define BOUNDINGVOLUME_H_

#include "Ray.h"
#include "Vector3F.h"
#include "Transform.h"

#include <vector>

struct BoundingVolume
{
	virtual ~BoundingVolume()
	{
	}

	virtual bool Compute(const std::vector<Vector3F>& rPoints) = 0;
	virtual bool Intersect(const Ray& rRay) const = 0;
	virtual void Update(Transform transform) = 0;

protected:
	BoundingVolume()
	{
	}

};

#endif