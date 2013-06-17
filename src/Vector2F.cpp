#include "Vector2F.h"

Vector2F operator + (float scalar, const Vector2F& rVector)
{
	return Vector2F(rVector.x + scalar, rVector.y + scalar);
}

Vector2F operator - (float scalar, const Vector2F& rVector)
{
	return Vector2F(rVector.x - scalar, rVector.y - scalar);
}

Vector2F operator * (float scalar, const Vector2F& rVector)
{
	return Vector2F(rVector.x * scalar, rVector.y * scalar);
}

Vector2F operator / (float scalar, const Vector2F& rVector)
{
	return Vector2F(rVector.x / scalar, rVector.y / scalar);
}

Vector2F operator - (const Vector2F& rVector)
{
	return Vector2F(-rVector.x, -rVector.y);
}