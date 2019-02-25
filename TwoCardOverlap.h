#pragma once
#include "HoleCards.h"
#include "Counts.h"

class TwoCardOverlap
{
public:
	static uint128_t Compute(const vector<uint64_t>& twoCards, int remaining, int opponents);

private:
	static Counts Compute2(const uint64_t* begin, const uint64_t* end);
	static Counts Compute3(const uint64_t* begin, const uint64_t* end);
	static Counts Compute4(const uint64_t* begin, const uint64_t* end);
	static Counts Compute5(const uint64_t* begin, const uint64_t* end);
	static Counts Compute6(const uint64_t* begin, const uint64_t* end);
	static Counts Compute7(const uint64_t* begin, const uint64_t* end);
	static Counts Compute8(const uint64_t* begin, const uint64_t* end);
};
