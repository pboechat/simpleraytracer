#include "Vector3F.h"

Vector3F operator + (float scalar, const Vector3F& rVector)
{
	return Vector3F(rVector.x + scalar, rVector.y + scalar, rVector.z + scalar);
}

Vector3F operator - (float scalar, const Vector3F& rVector)
{
	return Vector3F(rVector.x - scalar, rVector.y - scalar, rVector.z - scalar);
}

Vector3F operator * (float scalar, const Vector3F& rVector)
{
	return Vector3F(rVector.x * scalar, rVector.y * scalar, rVector.z * scalar);
}

Vector3F operator / (float scalar, const Vector3F& rVector)
{
	return Vector3F(rVector.x / scalar, rVector.y / scalar, rVector.z / scalar);
}

Vector3F operator - (const Vector3F& rVector)
{
	return Vector3F(-rVector.x, -rVector.y, -rVector.z);
}