#include "CommandPrompt.h"
#include "StringUtils.h"
#include "Application.h"
#include "Vector2F.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <vector>

//////////////////////////////////////////////////////////////////////////
CommandPrompt::CommandPrompt() :
	mOpen(false),
	mRunning(false),
	mParseCommand(false)
{
}

//////////////////////////////////////////////////////////////////////////
CommandPrompt::~CommandPrompt()
{
}

//////////////////////////////////////////////////////////////////////////
void CommandPrompt::OnFinish()
{
	mRunning = false;
}

//////////////////////////////////////////////////////////////////////////
void CommandPrompt::ShowMessage(const char* pFormat, ...)
{
	Open();

	std::cout << std::endl;

	va_list args;
	va_start(args, pFormat);
	vprintf(pFormat, args);
	va_end(args);

	std::cout << std::endl << std::endl << "> ";
}

//////////////////////////////////////////////////////////////////////////
void CommandPrompt::Toggle()
{
	if (mOpen)
	{
		Close();
	}
	else
	{
		Open();
	}
}

//////////////////////////////////////////////////////////////////////////
bool CommandPrompt::IsValidCharacter(char c)
{
	if (c == 95) // _
	{
		return true;
	}

	if (c == 32) // space
	{
		return true;
	}

	if (c >= 97 && c <= 122) // a-z
	{
		return true;
	}

	if (c >= 48 && c <= 57) // 0-9
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
void CommandPrompt::Open()
{
	if (mOpen)
	{
		return;
	}

	AllocConsole();

	HANDLE stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long) stdOutputHandle, _O_TEXT);
	FILE* hfOut = _fdopen(hCrt, "w");
	setvbuf(hfOut, NULL, _IONBF, 1);
	*stdout = *hfOut;

	HANDLE stdInputHandle = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long) stdInputHandle, _O_TEXT);
	FILE* hfIn = _fdopen(hCrt, "r");
	setvbuf(hfIn, NULL, _IONBF, 128);
	*stdin = *hfIn;

	std::cout << "> ";

	mOpen = true;
}

//////////////////////////////////////////////////////////////////////////
void CommandPrompt::Close()
{
	if (!mOpen)
	{
		return;
	}

	FreeConsole();

	mOpen = false;
}

//////////////////////////////////////////////////////////////////////////
DWORD CommandPrompt::Run()
{
	mRunning = true;
	while (mRunning)
	{
		if (!mOpen)
		{
			continue;
		}

		DWORD charsRead;
		INPUT_RECORD inputRecords[128];
		ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &inputRecords[0], 128, &charsRead);

		for (unsigned int i = 0; i < charsRead; i++)
		{
			if (inputRecords[i].EventType == KEY_EVENT)
			{
				const KEY_EVENT_RECORD& event = inputRecords[i].Event.KeyEvent;
				const int vk = event.wVirtualKeyCode;
				const char c = event.uChar.AsciiChar;
				if (!event.bKeyDown)
				{
					continue;
				}

				if (vk == VK_UP || vk == VK_DOWN)
				{
					for (unsigned int j = 0; j < mCommand.size(); j++)
					{
						std::cout << '\b';
					}

					for (unsigned int j = 0; j < mCommand.size(); j++)
					{
						std::cout << ' ';
					}

					for (unsigned int j = 0; j < mCommand.size(); j++)
					{
						std::cout << '\b';
					}

					std::cout << mLastCommand;
					std::string tmp = mCommand;
					mCommand = mLastCommand;
					mLastCommand = tmp;

					continue;
				}

				if (c == 0)
				{
					continue;
				}
				
				if (c == 8)
				{
					if (mCommand.empty())
					{
						continue;
					}

					mCommand = mCommand.substr(0, mCommand.size() - 1);
					std::cout << '\b' << ' ' << '\b';
				}
				else 
				{
					if (c == 13) // return
					{
						mParseCommand = true;
						std::cout << std::endl;
					}
					else if (IsValidCharacter(c))
					{
						mCommand += c;
						std::cout << c;
					}
				}
			}
		}

		if (mParseCommand)
		{
			ParseCommand();
			mLastCommand = mCommand;
			mCommand.clear();
			std::cout << std::endl << std::endl << "> ";
			mParseCommand = false;
		}
	}

	Close();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
void CommandPrompt::ParseCommand()
{
	StringUtils::Trim(mCommand);

	if (mCommand.compare(0, 10, "debug_mode") == 0)
	{
		std::vector<std::string> tokens;

		StringUtils::Tokenize(mCommand, " ", tokens);

		if (tokens.size() != 2)
		{
			std::cout << "debug_ray: Invalid number of arguments.";
			return;
		}

		if (tokens[1] == "off")
		{
			Application::GetInstance()->DisableDebugMode();
		}
		else if (tokens[1] == "on")
		{
			Application::GetInstance()->EnableDebugMode();
		}
		else
		{
			std::cout << "debug_mode: Invalid option.";
			return;
		}
	}
	else if (mCommand.compare(0, 9, "debug_ray") == 0)
	{
		std::vector<std::string> tokens;

		StringUtils::Tokenize(mCommand, " ", tokens);

		if (tokens.size() < 2)
		{
			std::cout << "debug_ray: Invalid number of arguments.";
			return;
		}

		if (tokens.size() == 2)
		{
			if (tokens[1] == "off")
			{
				Application::GetInstance()->DisableRayDebugging();
			}
			else
			{
				std::cout<< "debug_ray: Invalid argument.";
			}
		}
		else 
		{
			int x = atoi(tokens[1].c_str());
			int y = atoi(tokens[2].c_str());

			if (x == 0 || y == 0)
			{
				std::cout << "debug_ray: Invalid ray position. Ray position must be between 1 and width/height.";
				return;
			}

			Application::GetInstance()->EnableRayDebugging(Vector2F((float)x, (float)y));
		}
	}
	else 
	{
		std::cout << "Unknown command.";
	}
}
