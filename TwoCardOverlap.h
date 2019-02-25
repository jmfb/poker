#pragma once
#include "HoleCards.h"
#include "Counts.h"
#include "CombinationSequences.h"

class TwoCardOverlap
{
public:
	static uint128_t Compute(const vector<uint64_t>& twoCards, int remaining, int opponents);

private:
	static void Compute2(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence);
	static void Compute3(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence);
	static void Compute4(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence);
	static void Compute5(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence);
	static void Compute6(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence);
	static void Compute7(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence);
	static void Compute8(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence);
};
