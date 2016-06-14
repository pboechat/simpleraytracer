#ifndef TEXTURELOADER_H_
#define TEXTURELOADER_H_

#include <memory>
#include <string>
#include <vector>
#include <cassert>

#include "PicoPNG.h"
#include "FileReader.h"
#include "Texture.h"

class TextureLoader
{
public:
	static std::unique_ptr<Texture> LoadFromPNG(const std::string& rFileName)
	{
		size_t fileSize;
		auto encodedData = FileReader::Read<unsigned char>(rFileName, fileSize, FileMode::FM_BINARY);
		unsigned long width;
		unsigned long height;
		std::unique_ptr<unsigned char[]> decodedData;
		{
			std::vector<unsigned char> decodedData0;
			if (PicoPNG::decodePNG(decodedData0, width, height, encodedData.get(), fileSize, true) != 0)
				throw std::runtime_error("could not decode PNG file: " + rFileName);
			size_t size = decodedData0.size();
			assert(size > 0);
			auto pDecodedData = new unsigned char[size];
			memcpy(pDecodedData, &decodedData0[0], sizeof(unsigned char) * size);
			decodedData = std::unique_ptr<unsigned char[]>(pDecodedData);
		}
		return std::unique_ptr<Texture>(new Texture(width, height, std::move(decodedData)));
	}

private:
	TextureLoader() = default;

};

#endif