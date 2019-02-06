#include "pch.h"
#include "Math.h"

LargeInteger Combinations(LargeInteger total, LargeInteger take)
{
	LargeInteger result{ 1 };
	for (LargeInteger index{ 0 }; index < take; ++index)
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
LargeInteger Partitions(LargeInteger count)
{
	//Partitions(n) = Product(2k+1) for k=0..n-1 where n is number of opponents
	//count = opponent cards -> opponents = opponent cards / 2
	auto n = count / 2;
	LargeInteger product = 1;
	for (LargeInteger k = 0; k < n; ++k)
		product *= 2 * k + 1;
	return product;
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
	{
		if (iter->second == 0)
			cout << "WTF!\n";
		return iter->second;
	}
	auto result = Combinations(cards, opponentCards) * Partitions(opponentCards);
	totalCombinationCache.emplace(key, result);
	if (result == 0)
		cout << "OMFGWTF!\n";
	return result;
}
