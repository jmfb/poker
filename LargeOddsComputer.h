#pragma once
#include "AllHands.h"
#include "HoleCards.h"
#include "LargeOdds.h"
#include "CardSet.h"
#include "Card.h"
#include "Suit.h"

class LargeOddsComputer
{
public:
	LargeOddsComputer(const AllHands& allHands, const HoleCards& hole, int opponents);
	LargeOddsComputer(const LargeOddsComputer& rhs) = default;
	LargeOddsComputer(LargeOddsComputer&& rhs) = default;
	~LargeOddsComputer() = default;

	LargeOddsComputer& operator=(const LargeOddsComputer& rhs) = default;
	LargeOddsComputer& operator=(LargeOddsComputer&& rhs) = default;

	const LargeOdds& GetOdds() const;

private:
	LargeOdds ComputeCommunity(
		const AllHands& allHands,
		const HoleCards& hole,
		int c1,
		int c2,
		int c3,
		int c4,
		int c5,
		int opponents);
	static LargeInteger ComputeTwoCardOverlap(
		const CardSet& holes,
		vector<HoleCards>::const_iterator begin,
		vector<HoleCards>::const_iterator end,
		LargeInteger remaining,
		LargeInteger opponentCards);
	void AddSuitCombinations(const HoleCards& hole, Card c1, Card c2, Card c3, Card c4, Card c5, const LargeOdds& largeOdds);
	void AddSuitCombination(Card c1, Card c2, Card c3, Card c4, Card c5, const LargeOdds& largeOdds, const map<Suit, Suit>& suitMap);

private:
	map<int, LargeOdds> cache;
	LargeOdds odds;
};