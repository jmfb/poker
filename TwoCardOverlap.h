#pragma once
#include "HoleCards.h"
#include "Counts.h"
#include "Bitset.h"

class TwoCardOverlap
{
public:
	static uint128_t Compute(const vector<uint64_t>& twoCards, int remaining, int opponents);

private:
	static void Compute2(const vector<Bitset>& m, vector<Counts>& counts);
	static void Compute3(const vector<Bitset>& m, vector<Counts>& counts);
	static void Compute4(const vector<Bitset>& m, vector<Counts>& counts);
	static void Compute5(const vector<Bitset>& m, vector<Counts>& counts);
	static void Compute6(const vector<Bitset>& m, vector<Counts>& counts);
	static void Compute7(const vector<Bitset>& m, vector<Counts>& counts);
	static void Compute8(const vector<Bitset>& m, vector<Counts>& counts);
};
