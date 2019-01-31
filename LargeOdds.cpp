#include "pch.h"
#include "LargeOdds.h"
#include "Deck.h"
#include "Math.h"

LargeOdds::LargeOdds(const LargeInteger& winOrDraw, const LargeInteger& lose)
	: winOrDraw{ winOrDraw }, lose{ lose }
{
}

const LargeInteger& LargeOdds::GetWinOrDraw() const
{
	return winOrDraw;
}

const LargeInteger& LargeOdds::GetLose() const
{
	return lose;
}

LargeInteger LargeOdds::GetTotal() const
{
	return winOrDraw + lose;
}

double LargeOdds::GetWinOrDrawPercent() const
{
	return static_cast<int>(winOrDraw * 10000 / (winOrDraw + lose)) / 100.0;
}

LargeOdds LargeOdds::Create(const LargeInteger& winOrDraw, int opponents)
{
	return { winOrDraw, ComputeTotalCombinations(DeckSize - 5 - 2, 2 * opponents) - winOrDraw };
}

LargeOdds& LargeOdds::operator+=(const LargeOdds& rhs)
{
	winOrDraw += rhs.winOrDraw;
	lose += rhs.lose;
	return *this;
}
