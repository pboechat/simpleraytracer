#ifndef LIGHT_H_
#define LIGHT_H_

#include "Vector3F.h"

class Light
{
public:
	Color3F ambientColor;
	Color3F diffuseColor;
	Color3F specularColor;
	float intensity;

protected:
	Light() :
		ambientColor(0, 0, 0),
		diffuseColor(1, 1, 1),
		specularColor(1, 1, 1),
		intensity(1)
	{
	}

	Light(const Color3F& rAmbientColor, const Color3F& rDiffuseColor, const Color3F& rSpecularColor, float intensity) :
		ambientColor(rAmbientColor),
		diffuseColor(rDiffuseColor),
		specularColor(rSpecularColor)
	{
		this->intensity = intensity;
	}

};

#endif