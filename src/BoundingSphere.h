#ifndef BOUNDINGSPHERE_H_
#define BOUNDINGSPHERE_H_

#include "BoundingVolume.h"
#include "Common.h"

struct BoundingSphere : public BoundingVolume
{
public:
	Vector3F center;
	float radius;

	BoundingSphere() :
		radius(0),
		mCurrentRadius(0)
	{
	}

	virtual ~BoundingSphere()
	{
	}

	virtual bool Compute(const std::vector<Vector3F>& rPoints)
	{
		for (unsigned int i = 0; i < rPoints.size(); i++)
		{
			center += rPoints[i];
		}
		center /= (float)rPoints.size();

		for (unsigned int i = 0; i < rPoints.size(); i++)
		{
			float distance = center.Distance(rPoints[i]);
			if (distance > radius)
			{
				radius = distance;
			}
		}

		return true;
	}

	virtual bool Intersect(const Ray& rRay) const
	{
		Vector3F viewerDirection = rRay.origin - mCurrentCenter;

		float a = rRay.direction.Dot(rRay.direction);
		float b = 2.0f * rRay.direction.Dot(viewerDirection);
		float c = viewerDirection.Dot(viewerDirection) - (mCurrentRadius * mCurrentRadius);

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

		return (t > 0);
	}

	virtual void Update(Transform transform)
	{
		mCurrentCenter = transform * center;
		mCurrentRadius = radius * max(transform.scale[0][0], max(transform.scale[1][1], transform.scale[2][2]));
	}

private:
	Vector3F mCurrentCenter;
	float mCurrentRadius;

};

#endif