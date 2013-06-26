#ifndef SPHERE_H_
#define SPHERE_H_

#include "SceneObject.h"

#include <cmath>

#define PI 3.14159265359f

struct Sphere : public SceneObject
{
	float radius;

	Sphere(float radius)
	{
		this->radius = radius;
	}

	virtual ~Sphere()
	{
	}

	virtual bool Intersect(const Ray& rRay, RayHit& rHit) const
	{
		Vector3F viewerDirection = rRay.origin - mWorldTransform.position;

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
		rHit.normal = (rHit.point - mWorldTransform.position).Normalized();

		// Spherical Mapping with Normals:
		// http://www.mvps.org/directx/articles/spheremap.htm
		rHit.uv = Vector2F(asin(rHit.normal.x()) / PI + 0.5f, asin(rHit.normal.y()) / PI + 0.5f);

		return true;
	}

};

#endif