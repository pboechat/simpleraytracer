#ifndef TIME_H_
#define TIME_H_

#include <Windows.h>

#include <exception>

class Time
{
public:
	static double Now()
	{
		static double frequency = -1.0;
		LARGE_INTEGER time;

		if (frequency == -1.0)
		{
			if (!QueryPerformanceFrequency(&time))
			{
				throw std::exception("Cannot query performance counter frequency");
			}

			frequency = (double) time.QuadPart;
		}

		if(!QueryPerformanceCounter(&time))
		{
			throw std::exception("Cannot query performance counter");
		}

		return (double)time.QuadPart / frequency;
	}

private:
	Time() {}
	~Time() {}

};

#endif
