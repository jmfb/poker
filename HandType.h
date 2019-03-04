#pragma once

enum class HandType
{
	HighCard = 0,
	OnePair,
	TwoPair,
	ThreeOfAKind,
	Straight,
	Flush,
	FullHouse,
	FourOfAKind,
	StraightFlush,
	RoyalFlush
};

string ToString(HandType value);
