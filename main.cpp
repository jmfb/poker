#include "pch.h"
#include "AllHands.h"
#include "HoleCards.h"
#include "Card.h"
#include "Suit.h"
#include "Face.h"
#include "LargeOddsComputer.h"

int main()
{
	try
	{
		AllHands allHands;
		HoleCards hole{ Card{ Face::Ace, Suit::Spades }, Card{ Face::Jack, Suit::Spades } };
		LargeOddsComputer computer{ allHands, hole, 8 };
		cout << hole.ToString() << ": Win or draw " << computer.GetOdds().GetWinOrDraw() << ", Lose " << computer.GetOdds().GetLose() << '\n';
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
