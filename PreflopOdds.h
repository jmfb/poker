#pragma once
#include "HoleCards.h"

class PreflopOdds
{
public:
	PreflopOdds();
	PreflopOdds(const PreflopOdds& rhs) = default;
	PreflopOdds(PreflopOdds&& rhs) = default;
	~PreflopOdds() = default;

	PreflopOdds& operator=(const PreflopOdds& rhs) = default;
	PreflopOdds& operator=(PreflopOdds&& rhs) = default;

	double GetOdds(const HoleCards& hole, int opponents) const;
	int GetOddsRounded(const HoleCards& hole, int opponents) const;

private:
	map<int, map<int, double>> odds;
};
