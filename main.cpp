#include <iostream>
#include <exception>
using namespace std;

#include "HoleCards.h"

//2598960 possible hands.
//4 royal flushes
//36 straight flushes
//624 four of a kinds
//3744 full houses
//5108 flushes
//10200 straights
//54912 three of a kinds
//123552 two pairs
//1098240 pairs
//1302540 high cards

//1326 possible starting hands
//1225 other starting hands
//1712304 possible community combinations
//812175 distinct combinations of starting hands (1326 * 1225 / 2)

//1381800 Permutations for a single handle against two opponents.

//9 players is considered ideal table size.

int main()
{
	try
	{
		HoleCards hole1{ { Face::Ace, Suit::Spades }, { Face::Ace, Suit::Hearts } };
		auto count = 0;
		for (HoleCards hole2; hole2.IsValid(); hole2.MoveNext())
		{
			if (!hole2.IsDisjoint(hole1))
				continue;
			for (HoleCards hole3; hole3.IsValid(); hole3.MoveNext())
			{
				if (!hole3.IsDisjoint(hole1) || !hole3.IsDisjoint(hole2))
					continue;
				++count;
			}
		}
		cout << count << " Permutations for a single handle against two opponents.\n";
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
