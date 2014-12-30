#ifndef TEXTURELOADER_H_
#define TEXTURELOADER_H_

#include "PicoPNG.h"
#include "FileReader.h"
#include "Texture.h"

#include <string>
#include <vector>

class TextureLoader
{
public:
	static Texture* LoadFromPNG(const std::string& rFileName)
	{
		size_t fileSize;

		const unsigned char* pEncodedData = reinterpret_cast<const unsigned char*>(FileReader::Read(rFileName, fileSize, FileReader::FM_BINARY).data());
		std::vector<unsigned char>* pDecodedData = new std::vector<unsigned char>();
		unsigned long width;
		unsigned long height;
		PicoPNG::decodePNG(*pDecodedData, width, height, pEncodedData, fileSize, true);

		return new Texture(width, height, &(*pDecodedData)[0]);
	}

private:
	TextureLoader() {}
	~TextureLoader() {}

};

#endif