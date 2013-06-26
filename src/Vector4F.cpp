#include "Vector4F.h"

Vector4F operator + (float scalar, const Vector4F& rVector)
{
	scalar = scalar * rVector.w;
	return Vector4F(rVector.x + scalar, rVector.y + scalar, rVector.z + scalar, rVector.w);
}

Vector4F operator - (float scalar, const Vector4F& rVector)
{
	scalar = scalar * rVector.w;
	return Vector4F(rVector.x - scalar, rVector.y - scalar, rVector.z - scalar, rVector.w);
}

Vector4F operator * (float scalar, const Vector4F& rVector)
{
	scalar = scalar * rVector.w;
	return Vector4F(rVector.x * scalar, rVector.y * scalar, rVector.z * scalar, rVector.w);
}

Vector4F operator / (float scalar, const Vector4F& rVector)
{
	scalar = scalar * rVector.w;
	return Vector4F(rVector.x / scalar, rVector.y / scalar, rVector.z / scalar, rVector.w);
}

Vector4F operator - (const Vector4F& rVector)
{
	return Vector4F(-rVector.x, -rVector.y, -rVector.z, rVector.w);
}