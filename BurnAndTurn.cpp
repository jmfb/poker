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

string BurnAndTurn::ToString() const
{
	ostringstream out;
	for (auto index = 0; index < cards.size(); ++index)
		out << (index > 0 ? " " : "") << Card{ cards[index] }.ToString();
	return out.str();
}

int BurnAndTurn::operator[](int index) const
{
	return GetCard(index);
}
