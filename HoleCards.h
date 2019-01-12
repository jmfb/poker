#pragma once
#include "Card.h"
#include "Suit.h"
#include <map>
#include <string>
using namespace std;

class HoleCards
{
public:
	HoleCards() = default;
	HoleCards(const HoleCards& rhs) = default;
	HoleCards(HoleCards&& rhs) = default;
	HoleCards(int hash);
	HoleCards(const Card& card1, const Card& card2);
	HoleCards(Card card1, Card card2, const map<Suit, Suit>& suitMap);
	~HoleCards() = default;

	HoleCards& operator=(const HoleCards& rhs) = default;
	HoleCards& operator=(HoleCards&& rhs) = default;

	void MoveNext();
	bool IsValid() const;
	static int GetHash(int c1, int c2);
	int GetHash() const;
	void SetHash(int value);
	void SetCards(const Card& card1, const Card& card2);
	int GetCard1() const;
	int GetCard2() const;
	bool IsDisjoint(const HoleCards& rhs) const;
	bool Contains(int card) const;

	bool operator==(const HoleCards& rhs) const;
	bool operator!=(const HoleCards& rhs) const;

	string ToString() const;

private:
	int card1 = 0;
	int card2 = 1;
};
