#ifndef MESH_H_
#define MESH_H_

#include "SceneObject.h"
#include "Vector2F.h"
#include "BoundingVolume.h"

#include <vector>
#include <string>

#define EPSILON 0.000001f

struct Mesh : public SceneObject
{
	std::vector<Vector3F> vertices;
	std::vector<Vector3F> normals;
	std::vector<Vector2F> uvs;
	std::vector<unsigned int> indices;
	BoundingVolume* boundingVolume;

	Mesh() :
		boundingVolume(0)
	{
	}

	virtual ~Mesh()
	{
	}

	virtual void Update()
	{
		SceneObject::Update();

		if (boundingVolume != 0)
		{
			boundingVolume->Update(mWorldTransform);
		}
	}

	virtual bool Intersect(const Ray& rRay, RayHit& rHit) const
	{
		if (boundingVolume != 0 && !boundingVolume->Intersect(rRay))
		{
			return false;
		}

		for (unsigned int i = 0; i < indices.size(); i += 3)
		{
			unsigned int i1 = indices[i];
			unsigned int i2 = indices[i + 1];
			unsigned int i3 = indices[i + 2];

			Vector3F p1 = mWorldTransform * vertices[i1];
			Vector3F p2 = mWorldTransform * vertices[i2];
			Vector3F p3 = mWorldTransform * vertices[i3];

			float u, v, t;
			if (BackFaceCullTriangleIntersection(rRay, p1, p2, p3, &u, &v, &t))
			{
				rHit.point = rRay.origin + t * rRay.direction;

				float w = (1 - u - v);

				if (normals.size() > 0)
				{
					Vector3F n1 = mWorldTransform.rotation * normals[i1];
					Vector3F n2 = mWorldTransform.rotation * normals[i2];
					Vector3F n3 = mWorldTransform.rotation * normals[i3];

					rHit.normal = (w * n1 + u * n2 + v * n3).Normalized();
				}
				else
				{
					const Vector3F& rEdge1 = (p2 - p1);
					const Vector3F& rEdge2 = (p3 - p1);

					rHit.normal = rEdge1.Cross(rEdge2);
				}

				if (uvs.size() > 0)
				{
					const Vector2F& rUV1 = uvs[i1];
					const Vector2F& rUV2 = uvs[i2];
					const Vector2F& rUV3 = uvs[i3];
					rHit.uv = w * rUV1 + u * rUV2 + v * rUV3;
				}

				return true;
			}
		}

		return false;
	}

private:
	bool BackFaceCullTriangleIntersection(const Ray& rRay, const Vector3F& rP1, const Vector3F& rP2, const Vector3F& rP3, float* u, float* v, float* t) const
	{
		Vector3F& rEdge1 = (rP2 - rP1);
		Vector3F& rEdge2 = (rP3 - rP1);

		Vector3F& rPVec = rRay.direction.Cross(rEdge2);

		float determinant = rEdge1.Dot(rPVec);

		if (determinant < EPSILON)
		{
			return false;
		}

		Vector3F& rTVec = (rRay.origin - rP1);

		*u = rTVec.Dot(rPVec);

		if (*u < 0.0f || *u > determinant)
		{
			return false;
		}

		Vector3F& rQVec = rTVec.Cross(rEdge1);

		*v = rRay.direction.Dot(rQVec);

		if (*v < 0.0f || *u + *v > determinant)
		{
			return false;
		}

		*t = rEdge2.Dot(rQVec);

		float inverseDeterminant = 1.0f / determinant;

		*u *= inverseDeterminant;
		*v *= inverseDeterminant;
		*t *= inverseDeterminant;

		return true;
	}

};

#endif