#pragma once
#include "HoleCards.h"
#include "Counts.h"

class TwoCardOverlap
{
public:
	TwoCardOverlap() = default;
	TwoCardOverlap(const HoleCards& hole);
	TwoCardOverlap(const TwoCardOverlap& rhs) = default;
	TwoCardOverlap(TwoCardOverlap&& rhs) = default;
	~TwoCardOverlap() = default;

	TwoCardOverlap& operator=(const TwoCardOverlap& rhs) = default;
	TwoCardOverlap& operator=(TwoCardOverlap&& rhs) = default;

	uint64_t GetBits() const;

	void AddCount2();
	void AddCount3();
	void AddCount4();
	void AddCount5();
	void AddCount6();
	void AddCount7();
	void AddCount8();

	uint128_t GetTotalOverlap(uint128_t remainingCards, uint128_t opponentCards) const;

	TwoCardOverlap& operator+=(const TwoCardOverlap& rhs);

	static uint128_t Compute(const vector<HoleCards>& twoCards, int remaining, int opponents);

private:
	static void Compute2(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute3(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute4(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute5(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute6(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute7(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute8(TwoCardOverlap* begin, TwoCardOverlap* end);

private:
	uint64_t bits = 0;
	Counts counts;
};
