#ifndef SPHERE_H_
#define SPHERE_H_

#include "SceneObject.h"

class Sphere : public SceneObject
{
public:
	Vector3F center;
	float radius;

	Sphere(const Vector3F& rCenter, float radius)
	{
		this->center = rCenter;
		this->radius = radius;
	}

	~Sphere()
	{
	}

	virtual bool Intersect(const Ray& rRay, RayHit& rHit)
	{
		Vector3F viewerDirection = rRay.origin - center;

		float a = rRay.direction.Dot(rRay.direction);
		float b = 2.0f * rRay.direction.Dot(viewerDirection);
		float c = viewerDirection.Dot(viewerDirection) - (radius * radius);

		float delta = (b * b) - 4 * a * c;

		if (delta <= 0)
		{
			return false;
		}

		float a2 = a * 2.0f;
		float sqrtDelta = sqrt(delta);

		float t1 = (-b - sqrtDelta) / a2;
		float t2 = (-b + sqrtDelta) / a2;

		float t = (t1 < t2) ? t1 : t2;

		if (t <= 0)
		{
			return false;
		}

		rHit.point = rRay.origin + (t * rRay.direction);
		rHit.normal = (rHit.point - center).Normalized();

		return true;
	}

};

#endif