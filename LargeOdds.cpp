#include "pch.h"
#include "LargeOdds.h"
#include "Deck.h"
#include "Math.h"

LargeOdds::LargeOdds(int opponents, uint128_t winOrDraw, uint128_t lose)
	: opponents{ opponents }, winOrDraw{ winOrDraw }, lose{ lose }
{
}

int LargeOdds::GetOpponents() const
{
	return opponents;
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
	return static_cast<int>(winOrDraw * 10000 / GetTotal()) / 100.0;
}

uint128_t LargeOdds::GetCoinFlip() const
{
	return GetTotal() / (opponents + 1);
}

double LargeOdds::GetCoinFlipDeltaPercent() const
{
	auto coinFlip = GetCoinFlip();
	if (winOrDraw > coinFlip)
		return static_cast<int>((winOrDraw - coinFlip) * 10000 / GetTotal()) / 100.0;
	return -static_cast<int>((coinFlip - winOrDraw) * 10000 / GetTotal()) / 100.0;
}

LargeOdds LargeOdds::Create(uint128_t winOrDraw, int opponents)
{
	return { opponents, winOrDraw, ComputeTotalCombinations(DeckSize - 5 - 2, 2 * opponents) - winOrDraw };
}

LargeOdds& LargeOdds::operator+=(const LargeOdds& rhs)
{
	opponents = rhs.opponents;
	winOrDraw += rhs.winOrDraw;
	lose += rhs.lose;
	return *this;
}
