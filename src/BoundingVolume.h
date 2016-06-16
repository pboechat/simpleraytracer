#ifndef BOUNDINGVOLUME_H_
#define BOUNDINGVOLUME_H_

#include <vector>

#include "Ray.h"
#include "Vector3F.h"
#include "Transform.h"

struct BoundingVolume
{
	virtual ~BoundingVolume() = default;

	virtual bool Compute(const std::vector<Vector3F>& rPoints) = 0;
	virtual bool Intersect(const Ray& rRay) const = 0;
	virtual void Update(Transform transform) = 0;

protected:
	BoundingVolume() = default;

};

#endif