#ifndef LIGHT_H_
#define LIGHT_H_

#include "Vector3F.h"

class Light
{
public:
	Color3F diffuseColor;
	Color3F specularColor;
	float intensity;

protected:
	Light() :
		diffuseColor(1, 1, 1),
		specularColor(1, 1, 1),
		intensity(1)
	{
	}

	Light(const Color3F& rDiffuseColor, const Color3F& rSpecularColor, float intensity) :
		diffuseColor(rDiffuseColor),
		specularColor(rSpecularColor)
	{
		this->intensity = intensity;
	}

};

#endif