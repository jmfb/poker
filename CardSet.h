#pragma once
#include "Deck.h"
#include "HoleCards.h"

class CardSet
{
public:
	CardSet() = default;
	CardSet(const HoleCards& hole);
	CardSet(const CardSet& rhs) = default;
	CardSet(CardSet&& rhs) = default;
	~CardSet() = default;

	CardSet& operator=(const CardSet& rhs) = default;
	CardSet& operator=(CardSet&& rhs) = default;

	bool Intersect(const HoleCards& hole) const;
	CardSet Union(const HoleCards& hole) const;
	void AddCards(const HoleCards& hole);

private:
	array<bool, DeckSize> cards;
};
