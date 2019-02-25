#include "pch.h"
#include "Timer.h"
#include "HoleCards.h"
#include "TwoCardOverlap.h"
#include "Math.h"

vector<pair<int, int>> CreateTwoCards();

//Lose: 196 (1 opponents)
//Duration: 0.432ms
//Lose: 160419 (2 opponents)
//Duration: 1.107ms
//Lose: 59778861 (3 opponents)
//Duration: 0.875ms
//Lose: 13458144328 (4 opponents)
//Duration: 14.383ms (down from 25ms)
//Lose: 2048339780657 (5 opponents)
//Duration: 317.842mss (down from 450ms)
//Lose: 223478205275433 (6 opponents)
//Duration: 6266.72ms (6s down from 7s)
//Lose: 18084284371723401 (7 opponents)
//Duration: 95811.6ms (1.6m down from 1.7m)
//Lose: 1108007012317984113 (8 opponents)
//Duration: 875165ms (14.5m down from 15.8m)
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
