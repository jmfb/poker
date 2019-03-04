#include "pch.h"
#include "HandType.h"

string ToString(HandType value)
{
	switch (value)
	{
	case HandType::HighCard:
		return "High Card";
	case HandType::OnePair:
		return "One Pair";
	case HandType::TwoPair:
		return "Two Pair";
	case HandType::ThreeOfAKind:
		return "Three of a Kind";
	case HandType::Straight:
		return "Straight";
	case HandType::Flush:
		return "Flush";
	case HandType::FullHouse:
		return "Full House";
	case HandType::FourOfAKind:
		return "Four of a Kind";
	case HandType::StraightFlush:
		return "Straight Flush";
	case HandType::RoyalFlush:
		return "Royal Flush";
	}
	throw runtime_error{ "Invalid hand type." };
}
