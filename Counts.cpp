#include "pch.h"
#include "Counts.h"
#include "Math.h"

Counts& Counts::operator+=(const Counts& rhs)
{
	count2 += rhs.count2;
	count3 += rhs.count3;
	count4 += rhs.count4;
	count5 += rhs.count5;
	count6 += rhs.count6;
	count7 += rhs.count7;
	count8 += rhs.count8;
	return *this;
}

Counts Counts::operator+(const Counts& rhs) const
{
	auto result{ *this };
	result += rhs;
	return result;
}

void Counts::Dump() const
{
	cout << "Count2: " << count2 << '\n'
		<< "Count3: " << count3 << '\n'
		<< "Count4: " << count4 << '\n'
		<< "Count5: " << count5 << '\n';
	//NOTE: Could dump 6-8
}

Counts Counts::GetTotal(const vector<Counts>& counts)
{
	return reduce(execution::par_unseq, counts.begin(), counts.end(), Counts{}, [](auto lhs, auto rhs){ return lhs + rhs; });
}

uint128_t Counts::GetTotalOverlap(uint128_t remainingCards, uint128_t opponentCards) const
{
	return count2 * ComputeTotalCombinations(remainingCards, opponentCards) -
		count3 * ComputeTotalCombinations(remainingCards - 2, opponentCards - 2) +
		count4 * ComputeTotalCombinations(remainingCards - 4, opponentCards - 4) -
		count5 * ComputeTotalCombinations(remainingCards - 6, opponentCards - 6) +
		count6 * ComputeTotalCombinations(remainingCards - 8, opponentCards - 8) -
		count7 * ComputeTotalCombinations(remainingCards - 10, opponentCards - 10) +
		count8 * ComputeTotalCombinations(remainingCards - 12, opponentCards - 12);
}
