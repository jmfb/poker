#include "pch.h"
#include "Timer.h"

vector<pair<int, int>> CreateTwoCards();

//2-loop parallel size: 38416
//Count2: 16569
//Count3: 800361
//Count4: 24670043
//Count5: 513145502
//Duration: 463.116ms
void Test11()
{
	Timer timer;
	cout << "Test11: Standard combinations using bitfields and 2-loop parallelization.\n";

	vector<uint64_t> data;
	for (auto& twoCard : CreateTwoCards())
		data.push_back((1ull << twoCard.first) | (1ull << twoCard.second));

	struct counts_t
	{
		LargeInteger count2 = 0, count3 = 0, count4 = 0, count5 = 0;
		counts_t& operator+=(const counts_t& rhs)
		{
			count2 += rhs.count2;
			count3 += rhs.count3;
			count4 += rhs.count4;
			count5 += rhs.count5;
			return *this;
		}
		counts_t operator+(const counts_t& rhs) const
		{
			counts_t result{ *this };
			result += rhs;
			return result;
		}
		void Dump() const
		{
			cout << "Count2: " << count2 << '\n';
			cout << "Count3: " << count3 << '\n';
			cout << "Count4: " << count4 << '\n';
			cout << "Count5: " << count5 << '\n';
		}
	};
	auto size = data.size();
	auto begin = data.begin();
	auto countsSize = size * size;
	cout << "2-loop parallel size: " << countsSize << '\n';
	vector<counts_t> counts(countsSize);

	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(countsSize), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto remainder = index;
		auto index1 = remainder % size;
		remainder /= size;
		auto index2 = remainder % size;

		auto i1 = begin + index1;
		auto i2 = begin + index2;

		if (index2 >= index1)
			return;
		if ((*i1 & *i2) != 0)
			return;
		auto b2 = *i1 | *i2;
		++count.count2;

		for (auto i3 = begin; i3 != i2; ++i3)
		{
			if ((b2 & *i3) != 0)
				continue;
			auto b3 = b2 | *i3;
			++count.count3;
			for (auto i4 = begin; i4 != i3; ++i4)
			{
				if ((b3 & *i4) != 0)
					continue;
				auto b4 = b3 | *i4;
				++count.count4;
				for (auto i5 = begin; i5 != i4; ++i5)
				{
					if ((b4 & *i5) != 0)
						continue;
					++count.count5;
				}
			}
		}
	});

	reduce(execution::par_unseq, counts.begin(), counts.end(), counts_t{}, [](auto lhs, auto rhs)
	{
		return lhs + rhs;
	}).Dump();

	cout << "Duration: " << timer.GetDurationMs() << "ms\n";
}
