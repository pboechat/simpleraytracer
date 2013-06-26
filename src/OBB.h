#ifndef OBB_H_
#define OBB_H_

#include "BoundingVolume.h"
#include "Matrix3x3F.h"
#include "EigenSolver.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define swap(a, b) \
	{ \
		float tmp = (a); \
		(a) = (b); \
		(b) = tmp; \
	}

#define MAX_VALUE 100000.0f

struct OBB : public BoundingVolume
{
	Vector3F center;
	Vector3F minValues;
	Vector3F maxValues;
	Vector3F axis[3];

	virtual bool Compute(const std::vector<Vector3F>& rPoints)
	{
		Vector3F centroid;
		for (unsigned int i = 0; i < rPoints.size(); i++)
		{
			centroid += rPoints[i];
		}
		centroid /= (float)rPoints.size();

		Matrix3F covariance;
		for (unsigned int i = 0; i < rPoints.size(); i++)
		{
			Vector3F& rV1 = rPoints[i] - centroid;

			float m11 = rV1.x * rV1.x; float m12 = rV1.x * rV1.y; float m13 = rV1.x * rV1.z;
			float m21 = m12; float m22 = rV1.y * rV1.y; float m23 = rV1.y * rV1.z;
			float m31 = m13; float m32 = m23; float m33 = rV1.z * rV1.z;

			covariance += Matrix3F(m11, m12, m13,
								   m21, m22, m23,
								   m31, m32, m33);
		}

		EigenSolver solver(covariance);
		solver.SortDescreasingly();
		axis[0] = solver.GetEigenVector(0).Normalized();
		axis[1] = solver.GetEigenVector(1).Normalized();
		axis[2] = solver.GetEigenVector(2).Normalized();

		minValues = Vector3F(MAX_VALUE, MAX_VALUE, MAX_VALUE);
		maxValues = Vector3F(-MAX_VALUE, -MAX_VALUE, -MAX_VALUE);
		for (unsigned int i = 0; i < rPoints.size(); i++)
		{
			Vector3F vector(axis[0].Dot(rPoints[i]), axis[1].Dot(rPoints[i]), axis[2].Dot(rPoints[i]));

			minValues.x = min(vector.x, minValues.x);
			minValues.y = min(vector.y, minValues.y);
			minValues.z = min(vector.z, minValues.z);
			
			maxValues.x = max(vector.x, maxValues.x);
			maxValues.y = max(vector.y, maxValues.y);
			maxValues.z = max(vector.z, maxValues.z);
		}

		center = (maxValues + minValues) / 2.0f;

		return true;
	}

	virtual bool Intersect(const Ray& rRay) const
	{
		float t0 = 0.0f;
		float t1 = 1.0f;

		Vector3F rayOrigin(axis[0].Dot(rRay.origin), axis[1].Dot(rRay.origin), axis[2].Dot(rRay.origin));
		Vector3F rayDirection(axis[0].Dot(rRay.direction), axis[1].Dot(rRay.direction), axis[2].Dot(rRay.direction));

		if (!RaySlabIntersection(rayOrigin.x, rayDirection.x, mCurrentMinimums.x, mCurrentMaximums.x, t0, t1)) 
		{
			return false;
		}

		if (!RaySlabIntersection(rayOrigin.y, rayDirection.y, mCurrentMinimums.y, mCurrentMaximums.y, t0, t1)) 
		{
			return false;
		}
		
		if (!RaySlabIntersection(rayOrigin.z, rayDirection.z, mCurrentMinimums.z, mCurrentMaximums.z, t0, t1)) 
		{
			return false;
		}

		return true;
	}

	virtual void Update(Transform transform)
	{
		mCurrentCenter = transform * center;
	}

private:
	Vector3F mCurrentCenter;
	Vector3F mCurrentMinimums;
	Vector3F mCurrentMaximums;

	bool RaySlabIntersection(float start, float dir, float _min, float _max, float& t0, float& t1) const
	{
		if (fabs(dir) < 1.0E-8)
		{
			return (start < _max && start > _min);
		}

		float tmin = (_min - start) / dir;
		float tmax = (_max - start) / dir;
		if (tmin > tmax) swap(tmin, tmax);

		if (tmax < t0 || tmin > t1)
			return false;

		if (tmin > t0) t0 = tmin;
		if (tmax < t1)  t1  = tmax;

		return true;
	}

};

#endif