#include "pch.h"
#include "BurnAndTurn.h"

BurnAndTurn::BurnAndTurn(Deck& deck, int count)
{
	burn = deck.DealCard();
	for (auto index = 0; index < count; ++index)
		cards.push_back(deck.DealCard());
}

int BurnAndTurn::GetCard(int index) const
{
	if (index < 0 || index >= cards.size())
		throw runtime_error{ "Invalid index." };
	return cards[index];
}
