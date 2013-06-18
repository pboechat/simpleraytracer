#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "ColorRGBA.h"
#include "Texture.h"
#include "RenderAttributes.h"

struct Material
{
	ColorRGBA ambientColor;
	ColorRGBA diffuseColor;
	ColorRGBA specularColor;
	float shininess;
	Texture* texture;
	RenderAttributes renderAttributes;

	Material() :
		ambientColor(0, 0, 0, 0),
		diffuseColor(1, 1, 1, 1),
		specularColor(1, 1, 1, 1),
		shininess(1),
		texture(0)
	{
	}

	Material(const ColorRGBA& rAmbientColor, const ColorRGBA& rDiffuseColor, const ColorRGBA& rSpecularColor, float shininess, Texture* pTexture) :
		ambientColor(rAmbientColor),
		diffuseColor(rDiffuseColor),
		specularColor(rSpecularColor),
		texture(pTexture)
	{
		this->shininess = shininess;
	}

	~Material()
	{
		if (texture != 0)
		{
			delete texture;
		}
	}

};

#endif