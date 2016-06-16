#ifndef TIME_H_
#define TIME_H_

#include <stdexcept>

#include <Windows.h>

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
				throw std::runtime_error("Cannot query performance counter frequency");
			}

			frequency = (double) time.QuadPart;
		}

		if(!QueryPerformanceCounter(&time))
		{
			throw std::runtime_error("Cannot query performance counter");
		}

		return (double)time.QuadPart / frequency;
	}

private:
	Time() {}
	~Time() {}

};

#endif
