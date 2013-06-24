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
	void Start()
	{
		mThreadHandle = CreateThread(NULL, 0, &Runnable::RunThread, this, 0, NULL);
	}

	virtual void OnFinish()
	{
	}

	void Finish()
	{
		OnFinish();

		if (WaitForSingleObject(mThreadHandle, INFINITE) == WAIT_FAILED)
		{
			std::cerr << "Error waiting for thread to finish" << std::endl;
		}
	}

	virtual void Terminate()
	{
		TerminateThread(mThreadHandle, -1);
	}

	friend class Runnable;

protected:
	virtual DWORD Run() = 0;

private:
	HANDLE mThreadHandle;

};

#endif