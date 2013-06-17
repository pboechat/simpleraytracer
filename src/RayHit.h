#ifndef RAYHIT_H_
#define RAYHIT_H_

#include "Vector3F.h"
#include "Vector2F.h"

struct RayHit
{
	Vector3F point;
	Vector3F normal;
	Vector2F uv;

	RayHit()
	{
	}

	RayHit(const Vector3F& point, const Vector3F& normal)
	{
		this->point = point;
		this->normal = normal;
	}

	~RayHit()
	{
	}

};

#endif