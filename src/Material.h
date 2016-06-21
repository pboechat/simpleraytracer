#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <memory>

#include "ColorRGBA.h"
#include "Texture.h"

struct Material
{
	ColorRGBA diffuseColor;
	ColorRGBA specularColor;
	float shininess;
	bool transparent;
	float reflection;
	float refraction;
	std::unique_ptr<Texture> texture;

	Material() :
		diffuseColor(1, 1, 1, 1),
		specularColor(1, 1, 1, 1),
		shininess(1),
		transparent(false),
		reflection(false),
		refraction(false),
		texture(nullptr)
	{
	}

	Material(const ColorRGBA& rDiffuseColor, const ColorRGBA& rSpecularColor, float shininess, std::unique_ptr<Texture>& texture) :
		diffuseColor(rDiffuseColor),
		specularColor(rSpecularColor),
		shininess(shininess),
		transparent(false),
		reflection(false),
		refraction(false),
		texture(std::move(texture))
	{
	}

};

#endif