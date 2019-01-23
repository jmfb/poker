#pragma once
#include "AllHands.h"
#include "HoleCards.h"
#include "Odds.h"
#include "Suit.h"

class OddsCache
{
public:
	OddsCache() = default;
	OddsCache(const OddsCache& rhs) = default;
	OddsCache(OddsCache&& rhs) = default;
	~OddsCache() = default;

	OddsCache& operator=(const OddsCache& rhs) = default;
	OddsCache& operator=(OddsCache&& rhs) = default;

	void Load();
	void Save() const;
	void Compute(const AllHands& allHands);

private:
	void AddIfMissing(const AllHands& allHands, const HoleCards& hole1, const HoleCards& hole2);
	void PermuteHands(const HoleCards& hole1, const HoleCards& hole2, const Odds& odds);
	void PermuteHandsOneSuit(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const set<Suit>& suits);
	void PermuteHandsTwoSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const set<Suit>& suits);
	void PermuteHandsThreeSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const set<Suit>& suits);
	void PermuteHandsFourSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds);
	void PermuteSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const map<Suit, Suit>& suitMap);

private:
	map<tuple<int, int>, Odds> cache;
};
