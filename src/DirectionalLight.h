#ifndef DIRECTIONALLIGHT_H_
#define DIRECTIONALLIGHT_H_

#include "Light.h"

struct DirectionalLight : public Light
{
	Vector3F direction;

	DirectionalLight(const ColorRGBA& rDiffuseColor, const ColorRGBA& rSpecularColor, float intensity, const Vector3F& rDirection) :
		Light(rDiffuseColor, rSpecularColor, intensity)
	{
		direction = rDirection;
	}

	virtual ~DirectionalLight()
	{
	}

};

#endif