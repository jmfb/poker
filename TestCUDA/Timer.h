#pragma once

class Timer
{
public:
	Timer();
	Timer(const Timer& rhs) = default;
	Timer(Timer&& rhs) = default;
	~Timer() = default;

	Timer& operator=(const Timer& rhs) = default;
	Timer& operator=(Timer&& rhs) = default;

	double GetDurationMs() const;

private:
	time_point<system_clock> start;
};
