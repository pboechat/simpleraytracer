#ifndef COMMANDPROMPT_H_
#define COMMANDPROMPT_H_

#include "Thread.h"

#include <string>

class CommandPrompt : public Thread
{
public:
	CommandPrompt();
	virtual ~CommandPrompt();

	void Open();
	void Close();
	void Toggle();
	void ShowMessage(const char* pFormat, ...);
	virtual void OnFinish();

protected:
	virtual DWORD Run();

private:
	bool mOpen;
	bool mRunning;
	std::string mCommand;
	bool mParseCommand;

	void ParseCommand();

};

#endif