#pragma once

#include "BurnAndTurn.h"

class Community
{
public:
	Community() = default;
	Community(const BurnAndTurn& flop, const BurnAndTurn& turn, const BurnAndTurn& river);
	Community(const BurnAndTurn& flop, const BurnAndTurn& turn, int river);
	Community(const BurnAndTurn& flop, int turn, int river);
	Community(int c1, int c2, int c3, int c4, int c5);
	Community(const Community& rhs) = default;
	Community(Community&& rhs) = default;
	~Community() = default;

	Community& operator=(const Community& rhs) = default;
	Community& operator=(Community&& rhs) = default;

	int operator[](int index) const;

private:
	array<int, 5> cards;
};
