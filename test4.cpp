#include "pch.h"
#include "Timer.h"
#include "HoleCards.h"
#include "TwoCardOverlap.h"
#include "Math.h"

vector<pair<int, int>> CreateTwoCards();

//Lose: 196 (1 opponents)
//Duration: 0ms
//Lose: 160419 (2 opponents)
//Duration: 0ms
//Lose: 59778861 (3 opponents)
//Duration: 1ms
//Lose: 13458144328 (4 opponents)
//Duration: 25ms
//Lose: 2048339780657 (5 opponents)
//Duration: 450ms (364.691ms)
//Lose: 223478205275433 (6 opponents)
//Duration: 7478ms (7s)
//Lose: 18084284371723401 (7 opponents)
//Duration: 103488ms (1.7m)
//Lose: 1108007012317984113 (8 opponents)
//Duration: 948550ms (15.8m)
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
		auto overlap = TwoCardOverlap::Compute(holes, 45, opponents);

		auto lose = ComputeTotalCombinations(43, 2 * opponents - 2) * static_cast<long long>(twoCards.size()) - overlap;
		cout << "Lose: " << lose << " (" << opponents << " opponents)\n";

		cout << "Duration: " << timer.GetDurationMs() << "ms\n";
	}
	cout << '\n';
}
