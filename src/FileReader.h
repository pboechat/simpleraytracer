#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdexcept>

enum FileMode
{
	FM_TEXT, FM_BINARY
};

class FileReader
{
public:
	template <typename T>
	static std::unique_ptr<T[]> Read(const std::string& rFileName, FileMode mode, bool trailingEndOfString = false)
	{
		size_t unusedFileSize;
		return Read<T>(rFileName, unusedFileSize, mode, trailingEndOfString);
	}

	template <typename T>
	static std::unique_ptr<T[]> Read(const std::string& rFileName, size_t& fileSize, FileMode mode, bool trailingEndOfString = false)
	{
		std::unique_ptr<T[]> out(nullptr);
		fileSize = 0;
		if (!rFileName.empty())
		{
			FILE* file = fopen(rFileName.c_str(), GetFileModeString(mode));
			if (file == 0)
				throw std::runtime_error("file not found: " + rFileName);
			fseek(file, 0, SEEK_END);
			size_t size = ftell(file);
			fileSize = size;
			rewind(file);
			if (size == 0)
				throw std::runtime_error("empty file: " + rFileName);
			auto pBuffer = (T*)malloc(sizeof(T) * (size + trailingEndOfString));
			size = fread(pBuffer, sizeof(T), size, file);
			out = std::unique_ptr<T[]>(pBuffer);
			if (trailingEndOfString)
				out[size] = (T)'\0';
			fclose(file);
		}
		return out;
	}

private:
	FileReader() = default;

	static char* GetFileModeString(FileMode mode)
	{
		switch (mode)
		{
		case FM_TEXT:
			return "rt";

		case FM_BINARY:
			return "rb";

		default:
			return NULL;
		}
	}

};

#endif
