#include "pch.h"
#include "Timer.h"
#include "HoleCards.h"
#include "TwoCardOverlap.h"
#include "Math.h"

vector<pair<int, int>> CreateTwoCards();

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
void Test4()
{
	cout << "Test4: Standard combinations using bitfields and 1-loop parallelization.\n";
	auto opponents = 5;
	//for (auto opponents = 1; opponents <= 8; ++opponents)
	{
		Timer timer;

		auto twoCards = CreateTwoCards();
		vector<HoleCards> holes;
		for (auto& twoCard : twoCards)
			holes.emplace_back(twoCard.first, twoCard.second);
		auto overlap = TwoCardOverlap::Compute(ToBits(holes), 45, opponents);

		auto lose = ComputeTotalCombinations(43, 2 * opponents - 2) * static_cast<long long>(twoCards.size()) - overlap;
		cout << "Lose: " << lose << " (" << opponents << " opponents)\n";

		cout << "Duration: " << timer.GetDurationMs() << "ms\n";
	}
	cout << '\n';
}
