#include "pch.h"
#include "HoleCards.h"
#include "Deck.h"

HoleCards::HoleCards(int hash)
{
	SetHash(hash);
}

HoleCards::HoleCards(const Card& card1, const Card& card2)
{
	SetCards(card1, card2);
}

HoleCards::HoleCards(Card card1, Card card2, const map<Suit, Suit>& suitMap)
{
	card1.SetSuit(suitMap.find(card1.GetSuit())->second);
	card2.SetSuit(suitMap.find(card2.GetSuit())->second);
	SetCards(card1, card2);
}

void HoleCards::MoveNext()
{
	++card2;
	if (card2 < DeckSize)
		return;
	++card1;
	card2 = card1 + 1;
}

bool HoleCards::IsValid() const
{
	return card1 >= 0 && card1 < card2 && card2 < DeckSize;
}

int HoleCards::GetHash(int c1, int c2)
{
	auto hash = c1;
	hash *= DeckSize;
	hash += c2;
	return hash;
}

int HoleCards::GetHash() const
{
	return card1 * DeckSize + card2;
}

void HoleCards::SetHash(int value)
{
	card1 = value / DeckSize;
	card2 = value % DeckSize;
}

void HoleCards::SetCards(const Card& card1, const Card& card2)
{
	auto id1 = card1.GetId();
	auto id2 = card2.GetId();
	this->card1 = min(id1, id2);
	this->card2 = max(id1, id2);
}

int HoleCards::GetCard1() const
{
	return card1;
}

int HoleCards::GetCard2() const
{
	return card2;
}

bool HoleCards::IsDisjoint(const HoleCards& rhs) const
{
	return !rhs.Contains(card1) && !rhs.Contains(card2);
}

bool HoleCards::Contains(int card) const
{
	return card1 == card || card2 == card;
}

bool HoleCards::operator==(const HoleCards& rhs) const
{
	return card1 == rhs.card1 && card2 == rhs.card2;
}
bool HoleCards::operator!=(const HoleCards& rhs) const
{
	return !operator==(rhs);
}

string HoleCards::ToString() const
{
	return Card{ card1 }.ToString() + " " + Card{ card2 }.ToString();
}
