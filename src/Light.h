#ifndef LIGHT_H_
#define LIGHT_H_

#include "ColorRGBA.h"

struct Light
{
	ColorRGBA diffuseColor;
	ColorRGBA specularColor;
	float intensity;

	virtual ~Light()
	{
	}

protected:
	Light() :
		diffuseColor(1, 1, 1, 1),
		specularColor(1, 1, 1, 1),
		intensity(1)
	{
	}

	Light(const ColorRGBA& rDiffuseColor, const ColorRGBA& rSpecularColor, float intensity) :
		diffuseColor(rDiffuseColor),
		specularColor(rSpecularColor)
	{
		this->intensity = intensity;
	}

};

#endif