#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <memory>

#include "ColorRGBA.h"
#include "Texture.h"

struct Material
{
	ColorRGBA ambientColor;
	ColorRGBA diffuseColor;
	ColorRGBA specularColor;
	float shininess;
	std::unique_ptr<Texture> texture;
	bool transparent;
	float reflection;
	float refraction;

	Material() :
		ambientColor(1, 1, 1, 1),
		diffuseColor(1, 1, 1, 1),
		specularColor(1, 1, 1, 1),
		shininess(1),
		texture(nullptr)
	{
	}

	Material(const ColorRGBA& rAmbientColor, const ColorRGBA& rDiffuseColor, const ColorRGBA& rSpecularColor, float shininess, std::unique_ptr<Texture>& texture) :
		ambientColor(rAmbientColor),
		diffuseColor(rDiffuseColor),
		specularColor(rSpecularColor),
		texture(std::move(texture))
	{
		this->shininess = shininess;
	}

};

#endif