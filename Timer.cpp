#include "pch.h"
#include "Timer.h"

Timer::Timer()
	: start{ system_clock::now() }
{
}

double Timer::GetDurationMs() const
{
	return duration_cast<microseconds>(system_clock::now() - start).count() / 1000.0;
}

long long Timer::GetDurationS() const
{
	return duration_cast<seconds>(system_clock::now() - start).count();
}
