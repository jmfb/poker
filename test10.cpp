#include "pch.h"
#include "Timer.h"

vector<pair<int, int>> CreateTwoCards();

void Test10()
{
	Timer timer;
	cout << "Test10: Standard combinations using bitfields and 3-loop parallelization.\n";
	//TODO:
	cout << "Duration: " << timer.GetDurationMs() << "ms\n";
}
