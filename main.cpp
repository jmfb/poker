#include "pch.h"
#include "AllHands.h"
#include "HoleCards.h"
#include "Card.h"
#include "Suit.h"
#include "Face.h"
#include "LargeOddsComputer.h"

int main(int argc, char** argv)
{
	try
	{
		//return LargeOddsComputer::Compute(argc, argv);
		LargeOddsComputer::CombineOpponentOdds();
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
