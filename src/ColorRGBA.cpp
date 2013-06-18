#include "ColorRGBA.h"

ColorRGBA operator + (float scalar, const ColorRGBA& rVector)
{
	return ColorRGBA(rVector.r + scalar, rVector.g + scalar, rVector.b + scalar, rVector.a + scalar);
}

ColorRGBA operator - (float scalar, const ColorRGBA& rVector)
{
	return ColorRGBA(rVector.r - scalar, rVector.g - scalar, rVector.b - scalar, rVector.a - scalar);
}

ColorRGBA operator * (float scalar, const ColorRGBA& rVector)
{
	return ColorRGBA(rVector.r * scalar, rVector.g * scalar, rVector.b * scalar, rVector.a * scalar);
}

ColorRGBA operator / (float scalar, const ColorRGBA& rVector)
{
	return ColorRGBA(rVector.r / scalar, rVector.g / scalar, rVector.b / scalar, rVector.a / scalar);
}

ColorRGBA operator - (const ColorRGBA& rVector)
{
	return ColorRGBA(-rVector.r, -rVector.g, -rVector.b, -rVector.a);
}