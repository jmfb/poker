#pragma once
#include "Deck.h"

class BurnAndTurn
{
public:
	BurnAndTurn(Deck& deck, int count);
	BurnAndTurn(const BurnAndTurn& rhs) = default;
	BurnAndTurn(BurnAndTurn&& rhs) = default;
	~BurnAndTurn() = default;

	BurnAndTurn& operator=(const BurnAndTurn& rhs) = default;
	BurnAndTurn& operator=(BurnAndTurn&& rhs) = default;

	int GetCard(int index) const;
	string ToString() const;

	int operator[](int index) const;

private:
	int burn = 0;
	vector<int> cards;
};
