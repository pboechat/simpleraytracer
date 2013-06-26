#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

#include "Light.h"

struct PointLight : public Light
{
	Vector3F position;
	float attenuation;

	PointLight() :
		Light(),
		attenuation(1.0f)
	{
	}

	virtual ~PointLight()
	{
	}

	PointLight(const ColorRGBA& rDiffuseColor, const ColorRGBA& rSpecularColor, float intensity, const Vector3F& rPosition, float attenuation) :
	  Light(rDiffuseColor, rSpecularColor, intensity)
	{
		position = rPosition;
		this->attenuation = attenuation;
	}

};

#endif