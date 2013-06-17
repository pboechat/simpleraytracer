#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Vector3F.h"

struct Material
{
	Color3F ambientColor;
	Color3F diffuseColor;
	Color3F specularColor;
	float shininess;

	Material() :
		ambientColor(0, 0, 0),
		diffuseColor(1, 1, 1),
		specularColor(1, 1, 1),
		shininess(1)
	{
	}

	Material(const Color3F& rAmbientColor, const Color3F& rDiffuseColor, const Color3F& rSpecularColor, float shininess) :
		ambientColor(rAmbientColor),
		diffuseColor(rDiffuseColor),
		specularColor(rSpecularColor)
	{
		this->shininess = shininess;
	}

	~Material()
	{
	}

};

#endif