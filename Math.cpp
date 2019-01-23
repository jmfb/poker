#include "pch.h"
#include "Math.h"

LargeInteger Combinations(LargeInteger total, LargeInteger take)
{
	LargeInteger result{ 1 };
	for (LargeInteger index{ 0 }; index < take; ++index)
		result = result * (total - index) / (index + 1);
	return result;
}

LargeInteger Partitions(LargeInteger count)
{
	return count == 2 ? 1 : (count - 1) * Partitions(count - 2);
}

map<pair<LargeInteger, LargeInteger>, LargeInteger> totalCombinationCache;

LargeInteger ComputeTotalCombinations(LargeInteger cards, LargeInteger opponentCards)
{
	if (opponentCards < 0)
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
