#include "pch.h"
#include "Deck.h"
#include "AllHands.h"
#include "BurnAndTurn.h"
#include "Community.h"

Deck::iterator Deck::begin()
{
	return cards.begin();
}

Deck::iterator Deck::end()
{
	return begin() + size;
}

void Deck::Remove(iterator which)
{
	rotate(which, which + 1, end());
	--size;
}

void Deck::Remove(int card)
{
	auto e = end();
	auto iter = find(begin(), e, card);
	if (iter != e)
		Remove(iter);
}

void Deck::Remove(const HoleCards& hole)
{
	Remove(hole.GetCard1());
	Remove(hole.GetCard2());
}

void Deck::Remove(const HoleCards& hole, int c1, int c2, int c3, int c4, int c5)
{
	Remove(hole);
	Remove(c1);
	Remove(c2);
	Remove(c3);
	Remove(c4);
	Remove(c5);
}

void Deck::Remove(const BurnAndTurn& cards)
{
	for (auto card : cards)
		Remove(card);
}

void Deck::Remove(const Community& community)
{
	for (auto card : community)
		Remove(card);
}

void Deck::RemoveSingleCardLosses(const AllHands& allHands, int bestHand, int c1, int c2, int c3, int c4, int c5)
{
	for (auto iter = begin(); iter != end(); )
		if (allHands.BeatenByOneCard(bestHand, *iter, c1, c2, c3, c4, c5))
			Remove(iter);
		else
			++iter;
}

vector<HoleCards> Deck::FindTwoCardLosses(const AllHands& allHands, int bestHand, int c1, int c2, int c3, int c4, int c5)
{
	vector<HoleCards> losses;
	auto end = this->end();
	for (auto o1 = begin(); o1 != end; ++o1)
		for (auto o2 = o1 + 1; o2 != end; ++o2)
			if (allHands.BeatenByTwoCards(bestHand, *o1, *o2, c1, c2, c3, c4, c5))
				losses.emplace_back(*o1, *o2);
	return losses;
}

void Deck::Add(int card)
{
	if (size == DeckSize)
		throw runtime_error{ "Full deck." };
	cards[size++] = card;
}

int Deck::GetSize() const
{
	return size;
}

void Deck::Clear()
{
	size = 0;
}

void Deck::NewDeck()
{
	for (auto card = 0; card < DeckSize; ++card)
		cards[card] = card;
	size = DeckSize;
}

void Deck::Shuffle()
{
	::shuffle(begin(), end(), random_device{});
}

int Deck::DealCard()
{
	auto result = *begin();
	Remove(begin());
	return result;
}
