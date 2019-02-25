#include "pch.h"
#include "Counts.h"

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
