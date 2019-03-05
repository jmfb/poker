#include "pch.h"
#include "LargeOddsCache.h"

LargeOddsCache::LargeOddsCache(int opponents)
{
	Load(opponents);
}

void LargeOddsCache::Load(int opponents)
{
	this->opponents = opponents;
	cache.clear();
	ifstream in{ to_string(opponents) + "-opponent-odds.txt" };
	string line;
	while (getline(in, line))
	{
		istringstream parts{ line };
		int card1 = 0, card2 = 0;
		char comma = ',';
		string name;
		uint128_t winOrDraw = 0, lose = 0;
		parts >> card1 >> comma >> card2 >> comma;
		getline(parts, name, ',');
		parts >> winOrDraw >> comma >> lose;
		cache.emplace_back(HoleCards{ Card{ card1 }, Card{ card2 } }, LargeOdds{ opponents, winOrDraw, lose });
	}
	sort(cache.begin(), cache.end(), [](auto lhs, auto rhs){ return get<1>(lhs).GetWinOrDraw() > get<1>(rhs).GetWinOrDraw(); });
}

void LargeOddsCache::Analyze(ostream& out) const
{
	out << opponents << "-Opponent Starting Hand Analysis\n\n";
	out << "Rank,Opponents,Faces,Type,Card1,Card2,Hash,Label,WinOrDraw,Lose,Total,CoinFlip,WinOrDrawPercent,CoinFlipDeltaPercent\n";
	auto rank = 1;
	for (auto& hand : cache)
	{
		auto& hole = get<0>(hand);
		auto& odds = get<1>(hand);
		out << rank++ << ','
			<< opponents << ','
			<< hole.ToHandString() << ','
			<< hole.GetCard1() << ','
			<< hole.GetCard2() << ','
			<< hole.GetHash() << ','
			<< hole.ToString() << ','
			<< odds.GetWinOrDraw() << ','
			<< odds.GetLose() << ','
			<< odds.GetTotal() << ','
			<< odds.GetCoinFlip() << ','
			<< odds.GetWinOrDrawPercent() << ','
			<< odds.GetCoinFlipDeltaPercent() << '\n';
	}
}
