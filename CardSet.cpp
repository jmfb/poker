#include "pch.h"
#include "CardSet.h"

CardSet::CardSet()
{
	fill(cards.begin(), cards.end(), false);
}

CardSet::CardSet(const HoleCards& hole)
	: CardSet()
{
	AddCards(hole);
}

bool CardSet::Intersect(const HoleCards& hole) const
{
	return cards[hole.GetCard1()] || cards[hole.GetCard2()];
}

CardSet CardSet::Union(const HoleCards& hole) const
{
	auto result = *this;
	result.AddCards(hole);
	return result;
}

void CardSet::AddCards(const HoleCards& hole)
{
	cards[hole.GetCard1()] = true;
	cards[hole.GetCard2()] = true;
}
