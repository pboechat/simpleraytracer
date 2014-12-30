#ifndef FILEREADER_H_
#define FILEREADER_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <exception>

class FileReader
{
public:
	enum FileMode
	{
		FM_TEXT, FM_BINARY
	};

	static std::string Read(const std::string& rFileName, FileMode mode)
	{
		size_t unusedFileSize;
		return Read(rFileName, unusedFileSize, mode);
	}

	static std::string Read(const std::string& rFileName, size_t& fileSize, FileMode mode)
	{
		std::string out;
		fileSize = 0;

		if (!rFileName.empty())
		{
			FILE* file = fopen(rFileName.c_str(), GetFileModeString(mode));

			if (file == 0)
			{
				throw std::exception("file not found");
			}

			fseek(file, 0, SEEK_END);
			long size = ftell(file);
			fileSize = size;
			rewind(file);

			if (size > 0)
			{
				char* pBuffer = (char*) malloc(sizeof(char) * (size + 1));
				size = fread(pBuffer, sizeof(char), size, file);
				pBuffer[size] = '\0';
				out = std::string(pBuffer);
				delete pBuffer;
			}

			fclose(file);
		}

		return out;
	}

private:
	FileReader();
	~FileReader();

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
