#include "pch.h"
#include "Timer.h"
#include "HoleCards.h"
#include "TwoCardOverlap.h"
#include "Math.h"

vector<pair<int, int>> CreateTwoCards();

//Lose: 196 (1 opponents)
//Duration: 0.956ms
//Lose: 160419 (2 opponents)
//Duration: 0.353ms
//Lose: 59778861 (3 opponents)
//Duration: 0.479ms
//Lose: 13458144328 (4 opponents)
//Duration: 5.356ms (5ms down from 14ms from 25ms)
//Lose: 2048339780657 (5 opponents)
//Duration: 134.412ms (134ms down from 317ms from 450ms)
//Lose: 223478205275433 (6 opponents)
//Duration: 2142.24ms (2s down from 6s from 7s)
//Lose: 18084284371723401 (7 opponents)
//Duration: 31609.2ms (31s down from 95s from 102s)
//Lose: 1108007012317984113 (8 opponents)
//Duration: 355215ms (5.9m down from 14.5m from 15.8m)
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
