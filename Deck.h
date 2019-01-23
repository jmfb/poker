#pragma once
#include "Suit.h"
#include "Face.h"
#include "HoleCards.h"
#include "AllHands.h"

const auto DeckSize = SuitCount * FaceCount;

class Deck
{
public:
	Deck() = default;
	Deck(const Deck& rhs) = default;
	Deck(Deck&& rhs) = default;
	~Deck() = default;

	Deck& operator=(const Deck& rhs) = default;
	Deck& operator=(Deck&& rhs) = default;

	using iterator = array<int, DeckSize>::iterator;
	iterator begin();
	iterator end();
	void Remove(iterator which);
	void Remove(int card);
	void Remove(const HoleCards& hole);
	void Remove(const HoleCards& hole, int c1, int c2, int c3, int c4, int c5);
	void RemoveSingleCardLosses(const AllHands& allHands, int bestHand, int c1, int c2, int c3, int c4, int c5);
	vector<HoleCards> FindTwoCardLosses(const AllHands& allHands, int bestHand, int c1, int c2, int c3, int c4, int c5);
	void Add(int card);
	int GetSize() const;
	void Clear();
	void NewDeck();

private:
	int size = 0;
	array<int, DeckSize> cards;
};
