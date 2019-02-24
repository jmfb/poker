#include "pch.h"
#include "LargeOdds.h"
#include "Deck.h"
#include "Math.h"

LargeOdds::LargeOdds(uint128_t winOrDraw, uint128_t lose)
	: winOrDraw{ winOrDraw }, lose{ lose }
{
}

uint128_t LargeOdds::GetWinOrDraw() const
{
	return winOrDraw;
}

uint128_t LargeOdds::GetLose() const
{
	return lose;
}

uint128_t LargeOdds::GetTotal() const
{
	return winOrDraw + lose;
}

double LargeOdds::GetWinOrDrawPercent() const
{
	return static_cast<int>(winOrDraw * 10000 / (winOrDraw + lose)) / 100.0;
}

LargeOdds LargeOdds::Create(uint128_t winOrDraw, int opponents)
{
	return { winOrDraw, ComputeTotalCombinations(DeckSize - 5 - 2, 2 * opponents) - winOrDraw };
}

LargeOdds& LargeOdds::operator+=(const LargeOdds& rhs)
{
	winOrDraw += rhs.winOrDraw;
	lose += rhs.lose;
	return *this;
}
