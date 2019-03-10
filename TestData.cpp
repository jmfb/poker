#include "pch.h"
#include "TestData.h"

//
//Good example for testing optimizations to 2-card overlap computations.
//Flop{ 2s Tc 3s } Turn{ 8s } River{ Jc } Player0{ 9h Jd } - 196 2-card losses
//
vector<pair<int, int>> CreateTwoCardsPair()
{
	return
	{
		{ 2, 3 }, { 2, 4 }, { 2, 5 }, { 2, 7 }, { 2, 8 }, { 2, 9 }, { 2, 10 }, { 2, 11 }, { 2, 12 },
	{ 3, 4 }, { 3, 5 }, { 3, 7 }, { 3, 8 }, { 3, 9 }, { 3, 10 }, { 3, 11 }, { 3, 12 },
	{ 4, 5 }, { 4, 7 }, { 4, 8 }, { 4, 9 }, { 4, 10 }, { 4, 11 }, { 4, 12 },
	{ 5, 7 }, { 5, 8 }, { 5, 9 }, { 5, 10 }, { 5, 11 }, { 5, 12 }, { 5, 33 }, { 5, 46 },
	{ 7, 8 }, { 7, 9 }, { 7, 10 }, { 7, 11 }, { 7, 12 }, { 7, 18 }, { 7, 23 }, { 7, 31 }, { 7, 36 }, { 7, 44 }, { 7, 49 },
	{ 8, 9 }, { 8, 10 }, { 8, 11 }, { 8, 12 }, { 8, 13 }, { 8, 14 }, { 8, 19 }, { 8, 21 },
	{ 8, 22 }, { 8, 26 }, { 8, 27 }, { 8, 32 }, { 8, 39 }, { 8, 40 }, { 8, 45 }, { 8, 47 },
	{ 9, 10 }, { 9, 11 }, { 9, 12 }, { 9, 13 }, { 9, 14 }, { 9, 19 }, { 9, 21 }, { 9, 22 },
	{ 9, 23 }, { 9, 24 }, { 9, 25 }, { 9, 26 }, { 9, 27 }, { 9, 32 }, { 9, 36 }, { 9, 37 },
	{ 9, 38 }, { 9, 39 }, { 9, 40 }, { 9, 45 }, { 9, 47 }, { 9, 49 }, { 9, 50 }, { 9, 51 },
	{ 10, 11 }, { 10, 12 }, { 10, 22 }, { 10, 23 }, { 10, 33 }, { 10, 36 }, { 10, 46 }, { 10, 49 },
	{ 11, 12 }, { 11, 22 }, { 11, 24 }, { 11, 37 }, { 11, 50 },
	{ 12, 22 }, { 12, 25 }, { 12, 38 }, { 12, 51 },
	{ 13, 14 }, { 13, 19 }, { 13, 21 }, { 13, 22 }, { 13, 26 }, { 13, 27 }, { 13, 32 }, { 13, 39 }, { 13, 40 }, { 13, 45 }, { 13, 47 },
	{ 14, 19 }, { 14, 21 }, { 14, 22 }, { 14, 26 }, { 14, 27 }, { 14, 32 }, { 14, 39 }, { 14, 40 }, { 14, 45 }, { 14, 47 },
	{ 18, 33 }, { 18, 46 },
	{ 19, 21 }, { 19, 22 }, { 19, 26 }, { 19, 27 }, { 19, 32 }, { 19, 39 }, { 19, 40 }, { 19, 45 }, { 19, 47 },
	{ 21, 22 }, { 21, 26 }, { 21, 27 }, { 21, 32 }, { 21, 39 }, { 21, 40 }, { 21, 45 }, { 21, 47 },
	{ 22, 23 }, { 22, 24 }, { 22, 25 }, { 22, 26 }, { 22, 27 }, { 22, 32 }, { 22, 36 }, { 22, 37 },
	{ 22, 38 }, { 22, 39 }, { 22, 40 }, { 22, 45 }, { 22, 47 }, { 22, 49 }, { 22, 50 }, { 22, 51 },
	{ 23, 33 }, { 23, 36 }, { 23, 46 }, { 23, 49 },
	{ 24, 37 }, { 24, 50 },
	{ 25, 38 }, { 25, 51 },
	{ 26, 27 }, { 26, 32 }, { 26, 39 }, { 26, 40 }, { 26, 45 }, { 26, 47 },
	{ 27, 32 }, { 27, 39 }, { 27, 40 }, { 27, 45 }, { 27, 47 },
	{ 31, 33 }, { 31, 46 },
	{ 32, 39 }, { 32, 40 }, { 32, 45 }, { 32, 47 },
	{ 33, 36 }, { 33, 44 }, { 33, 49 },
	{ 36, 46 }, { 36, 49 },
	{ 37, 50 },
	{ 38, 51 },
	{ 39, 40 }, { 39, 45 }, { 39, 47 },
	{ 40, 45 }, { 40, 47 },
	{ 44, 46 },
	{ 45, 47 },
	{ 46, 49 }
	};
}

vector<HoleCards> CreateTwoCardsHole()
{
	vector<HoleCards> holes;
	for (auto& twoCard : CreateTwoCardsPair())
		holes.emplace_back(twoCard.first, twoCard.second);
	return holes;
}

vector<uint64_t> CreateTwoCardsBits()
{
	return ToBits(CreateTwoCardsHole());
}