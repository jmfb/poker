#include "pch.h"
#include "Timer.h"
#include "TwoCardOverlap.h"
#include "Math.h"
#include "TestData.h"

//Lose: 196 (1 opponents)
//Duration: 0.413ms
//Lose: 160419 (2 opponents)
//Duration: 0.417ms
//Lose: 59778861 (3 opponents)
//Duration: 0.443ms
//Lose: 13458144328 (4 opponents)
//Duration: 3.938ms (down from 5ms)
//Lose: 2048339780657 (5 opponents)
//Duration: 79.442ms (down from 134ms)
//Lose: 223478205275433 (6 opponents)
//Duration: 1460.73ms (down from 2.1s)
//Lose: 18084284371723401 (7 opponents)
//Duration: 21662.5ms (down from 31s)
//Lose: 1108007012317984113 (8 opponents)
//Duration: 264692ms (down from 355s; 4.4m < 5.9m)
void TestBaseline(int firstOpponents, int maxOpponents)
{
	for (auto opponents = firstOpponents; opponents <= maxOpponents; ++opponents)
	{
		Timer timer;
		auto twoCards = CreateTwoCardsBits();
		auto overlap = TwoCardOverlap::Compute(twoCards, 45, opponents);
		auto lose = ComputeTotalCombinations(43, 2 * opponents - 2) * static_cast<long long>(twoCards.size()) - overlap;
		cout << "Lose: " << lose << " (" << opponents << " opponents)\n";
		cout << "Duration: " << timer.GetDurationMs() << "ms\n";
	}
}
