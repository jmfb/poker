#pragma once
#include "LargeOdds.h"
#include "HoleCards.h"

class LargeOddsCache
{
public:
	LargeOddsCache() = default;
	LargeOddsCache(int opponents);
	LargeOddsCache(const LargeOddsCache& rhs) = default;
	LargeOddsCache(LargeOddsCache&& rhs) = default;
	~LargeOddsCache() = default;

	LargeOddsCache& operator=(const LargeOddsCache& rhs) = default;
	LargeOddsCache& operator=(LargeOddsCache&& rhs) = default;

	void Load(int opponents);

	void Analyze(ostream& out) const;

private:
	int opponents = 0;
	vector<tuple<HoleCards, LargeOdds>> cache;
};
