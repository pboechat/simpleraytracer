#ifndef DIRECTIONALLIGHT_H_
#define DIRECTIONALLIGHT_H_

#include "Light.h"

class DirectionalLight : public Light
{
public:
	Vector3F direction;

	DirectionalLight(const Color3F& rDiffuseColor, const Color3F& rSpecularColor, float intensity, const Vector3F& rDirection) :
		Light(rDiffuseColor, rSpecularColor, intensity)
	{
		direction = rDirection;
	}

};

#endif