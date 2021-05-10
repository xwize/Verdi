
#include "VerdiTimer.h"

#if defined(_WIN32) || defined(_MSC_VER)
#include <windows.h>
#else
#include <ctime>
#endif

namespace Verdi {

#if defined(_WIN32) || defined(_MSC_VER)
	uint64_t Timer::getTickCount()
	{
		uint64_t tval;
		QueryPerformanceCounter( (LARGE_INTEGER*)&tval );
		return tval;
	}
#else
	uint64_t Timer::getTickCount()
	{
		clock_t ct = clock();
		double dct = ( (double)ct / (double)CLOCKS_PER_SEC ) * 1000.0;
		return (uint64_t)dct;
	}
#endif

	Timer::Timer()
	{
		reset();
	}

	Timer::~Timer()
	{

	}

	void Timer::reset()
	{
		mFrequency = 1.0;
#if defined(_WIN32) || defined(_MSC_VER)
		uint64_t pf;
		QueryPerformanceFrequency( (LARGE_INTEGER*) &pf );
		mFrequency = 1000.0/(double)pf;
		QueryPerformanceCounter( (LARGE_INTEGER*)&mResetTime);
#else
		mResetTime = 0;
		mResetTime = getMilliseconds();
#endif

	}

	double Timer::getSeconds() const 
	{
		return (double)(getMilliseconds())/1000.0;
	}

	uint64_t Timer::getMilliseconds() const
	{
		uint64_t currTime = getTickCount();
		double temp = (currTime - mResetTime) * mFrequency;
		return (uint64_t)temp;
	}
}