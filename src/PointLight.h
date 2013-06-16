#ifndef POINTLIGHT_H_
#define POINTLIGHT_H_

#include "Light.h"

class PointLight : public Light
{
public:
	Vector3F position;
	float attenuation;

	PointLight() :
		Light(),
		attenuation(1.0f)
	{
	}

	PointLight(const Color3F& rAmbientColor, const Color3F& rDiffuseColor, const Color3F& rSpecularColor, float intensity, const Vector3F& rPosition, float attenuation) :
	  Light(rAmbientColor, rDiffuseColor, rSpecularColor, intensity)
	{
		position = rPosition;
		this->attenuation = attenuation;
	}

};

#endif