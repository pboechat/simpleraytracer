#ifndef TIME_H_
#define TIME_H_

#include <Windows.h>

#include <exception>

class Time
{
public:
	static double Now()
	{
		LARGE_INTEGER time;

		if (QueryPerformanceCounter(&time))
		{
			return (double)time.QuadPart * GetSecondsPerTick();
		}

		else
		{
			throw std::exception("Cannot query performance counter frequency");
		}
	}

private:
	static double s_mSecondsPerTick;

	static double GetSecondsPerTick()
	{
		if (s_mSecondsPerTick == -1)
		{
			LARGE_INTEGER frequency;

			if (QueryPerformanceFrequency(&frequency))
			{
				s_mSecondsPerTick = 1.0 / frequency.QuadPart;
			}

			else
			{
				throw std::exception("Cannot query performance counter frequency");
			}
		}

		return s_mSecondsPerTick;
	}

	Time() {}
	~Time() {}

};

double Time::s_mSecondsPerTick = -1;

#endif
