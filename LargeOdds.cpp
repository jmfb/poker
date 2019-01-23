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

LargeOdds LargeOdds::Create(const LargeInteger& winOrDraw, int opponents)
{
	return { winOrDraw, ComputeTotalCombinations(DeckSize - 5 - 2, 2 * opponents) - winOrDraw };
}
