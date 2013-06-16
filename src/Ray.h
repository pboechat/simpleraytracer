#ifndef RAY_H_
#define RAY_H_

#include "Vector3F.h"

struct Ray
{
	Vector3F origin;
	Vector3F direction;

	Ray(const Vector3F& rOrigin, const Vector3F& rDirection) :
		origin(rOrigin),
		direction(rDirection)
	{
	}

	~Ray()
	{
	}

};

#endif