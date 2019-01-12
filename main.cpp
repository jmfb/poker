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

long long Combinations(long long total, long long take)
{
	long long result = 1;
	for (long long index = 0; index < take; ++index)
		result = result * (total - index) / (index + 1);
	return result;
}

long long Partitions(long long count)
{
	return count == 2 ? 1 : (count - 1) * Partitions(count - 2);
}

int main()
{
	try
	{
		//auto startingHands = Combinations(52, 2);
		//auto opponentCount = 8;
		//auto opponentCards = 2 * opponentCount;
		//auto opponentCombinations = Combinations(50, opponentCards);
		//auto partitions = Partitions(opponentCards);
		//auto communityCards = Combinations(50 - opponentCards, 5);

		cout << Combinations(52, 5) << endl;

		//Starting Hands : 1,326
		//Opponent Cards : 16
		//Opponent Combinations : 4,923,689,695,575
		//Partitions : 2,027,025
		//Community Cards : 278,256

		//3 octillion overflows 64 bit (would need a 128 bit)
		//auto omfg = startingHands * opponentCombinations * partitions * communityCards;
		//3,682,458,333,300,934,657,049,580,000

		//1,326 * 4,923,689,695,575 * 2,027,025 * 278,256
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
