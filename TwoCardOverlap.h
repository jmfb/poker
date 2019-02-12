#pragma once
#include "HoleCards.h"

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

	unsigned long long GetBits() const;

	void AddCount2();
	void AddCount3();
	void AddCount4();
	void AddCount5();
	void AddCount6();
	void AddCount7();
	void AddCount8();

	LargeInteger GetTotalOverlap(LargeInteger remainingCards, LargeInteger opponentCards) const;

	TwoCardOverlap& operator+=(const TwoCardOverlap& rhs);

	static LargeInteger Compute(const vector<HoleCards>& twoCards, int remaining, int opponents);

private:
	static void Compute2(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute3(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute4(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute5(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute6(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute7(TwoCardOverlap* begin, TwoCardOverlap* end);
	static void Compute8(TwoCardOverlap* begin, TwoCardOverlap* end);

private:
	unsigned long long bits = 0;
	LargeInteger count2 = 0;
	LargeInteger count3 = 0;
	LargeInteger count4 = 0;
	LargeInteger count5 = 0;
	LargeInteger count6 = 0;
	LargeInteger count7 = 0;
	LargeInteger count8 = 0;
};
