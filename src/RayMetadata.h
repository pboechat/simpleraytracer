#ifndef RAYMETADATA_H_
#define RAYMETADATA_H_

#include "Vector3F.h"

struct RayMetadata
{
	Vector3F start;
	Vector3F end;
	RayMetadata* next;

	RayMetadata() :
		end(-1, -1, -1),
		next(0)
	{
	}

	~RayMetadata()
	{
		if (next != 0)
		{
			delete next;
		}
	}

};

#endif