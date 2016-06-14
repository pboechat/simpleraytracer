#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <memory>

#include "ColorRGBA.h"
#include "Vector2F.h"

struct Texture
{
	unsigned long width;
	unsigned long height;
	std::unique_ptr<unsigned char[]> data;

	Texture() :
		width(0),
		height(0),
		data(nullptr)
	{
	}

	Texture(unsigned long width, unsigned long height, std::unique_ptr<unsigned char[]> data) : width(width), height(height), data(std::move(data))
	{
	}

	ColorRGBA Sample(const Vector2F& rUV)
	{
		unsigned int x = static_cast<unsigned int>(rUV.x() * (width - 1));
		unsigned int y = static_cast<unsigned int>((1 - rUV.y()) * (height - 1));
		unsigned int i = (y * width + x) * 4;
		unsigned char r = data[i];
		unsigned char g = data[i + 1];
		unsigned char b = data[i + 2];
		unsigned char a = data[i + 3];
		return ColorRGBA(static_cast<float>(r / 255.0), static_cast<float>(g / 255.0), static_cast<float>(b / 255.0), static_cast<float>(a / 255.0));
	}

};

#endif