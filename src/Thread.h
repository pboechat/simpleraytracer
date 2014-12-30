#ifndef THREAD_H_
#define THREAD_H_

#include <windows.h>
#include <iostream>

class Runnable
{
public:
	virtual ~Runnable() {}

	static DWORD WINAPI RunThread(void* args)
	{
		Runnable* pRunnable = static_cast<Runnable*>(args);
		return pRunnable->Run();
	}

protected:
	virtual DWORD Run() = 0;

};

class Thread : public Runnable
{
public:
	Thread() : mThreadHandle(0)
	{
	}

	virtual ~Thread()
	{
	}

	void Start()
	{
		mThreadHandle = CreateThread(NULL, 0, &Runnable::RunThread, this, 0, NULL);
	}

	virtual void Stop()
	{
	}

	virtual void Terminate(bool isInitiator = false)
	{
		Stop();
		if (WaitForSingleObject(mThreadHandle, INFINITE) != WAIT_OBJECT_0)
		{
			std::cerr << "Error waiting for thread to finish" << std::endl;
		}
	}

	friend class Runnable;

protected:
	virtual DWORD Run() = 0;

private:
	HANDLE mThreadHandle;

};

#endif