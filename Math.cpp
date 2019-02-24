#include "pch.h"
#include "Math.h"
#include "Deck.h"

uint128_t Combinations(uint128_t total, uint128_t take)
{
	uint128_t result{ 1 };
	for (uint128_t index{ 0 }; index < take; ++index)
		result = result * (total - index) / (index + 1);
	return result;
}

//2 = 1
//4 = 3
//6 = 15
//8 = 105
//10 = 945
//12 = 10,395
//14 = 135,135
//16 = 2,027,025
uint128_t Partitions(uint128_t count)
{
	//Partitions(n) = Product(2k+1) for k=0..n-1 where n is number of opponents
	//count = opponent cards -> opponents = opponent cards / 2
	auto n = count / 2;
	uint128_t product{ 1 };
	for (uint128_t k{ 0 }; k < n; ++k)
		product *= 2 * k + 1;
	return product;
}

map<pair<uint128_t, uint128_t>, uint128_t> totalCombinationCache;

uint128_t ComputeTotalCombinations(uint128_t cards, uint128_t opponentCards)
{
	if (opponentCards > DeckSize)
		return 0;
	if (opponentCards == 0)
		return 1;
	auto key = make_pair(cards, opponentCards);
	auto iter = totalCombinationCache.find(key);
	if (iter != totalCombinationCache.end())
		return iter->second;
	auto result = Combinations(cards, opponentCards) * Partitions(opponentCards);
	totalCombinationCache.emplace(key, result);
	return result;
}
