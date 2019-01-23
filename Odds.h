#pragma once
#include "AllHands.h"
#include "HoleCards.h"

class Odds
{
public:
	Odds() = default;
	Odds(const Odds& rhs) = default;
	Odds(Odds&& rhs) = default;
	Odds(int win, int draw, int lose);
	~Odds() = default;

	Odds& operator=(const Odds& rhs) = default;
	Odds& operator=(Odds&& rhs) = default;

	void Compute(const AllHands& allHands, const HoleCards& hole1, const HoleCards& hole2);
	int GetWin() const;
	int GetDraw() const;
	int GetLose() const;
	Odds Invert() const;
	string ToString() const;

private:
	int win = 0;
	int draw = 0;
	int lose = 0;
};
