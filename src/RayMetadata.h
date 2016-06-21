#ifndef RAYMETADATA_H_
#define RAYMETADATA_H_

#include <memory>

#include "Vector3F.h"

struct RayMetadata
{
	Vector3F start;
	Vector3F direction;
	Vector3F end;
	bool isReflection;
	bool isRefraction;
	std::unique_ptr<RayMetadata> next;

	RayMetadata() :
		end(-1, -1, -1),
		next(nullptr),
		isReflection(false),
		isRefraction(false)
	{
	}

};

#endif