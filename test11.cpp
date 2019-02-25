#include "pch.h"
#include "Timer.h"
#include "Counts.h"

vector<pair<int, int>> CreateTwoCards();

//2-loop parallel size: 38416
//Count2: 16569
//Count3: 800361
//Count4: 24670043
//Count5: 513145502
//Duration: 306.783ms
//Sum: 2.395ms
void Test11()
{
	Timer timer;
	cout << "Test11: Standard combinations using bitfields and 2-loop parallelization.\n";

	vector<uint64_t> data;
	for (auto& twoCard : CreateTwoCards())
		data.push_back((1ull << twoCard.first) | (1ull << twoCard.second));

	auto size = data.size();
	auto begin = data.begin();
	auto countsSize = size * size;
	cout << "2-loop parallel size: " << countsSize << '\n';
	vector<Counts> counts(countsSize);

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
	cout << "Duration: " << timer.GetDurationMs() << "ms\n";

	Timer sumTimer;
	Counts::GetTotal(counts).Dump();
	cout << "Sum: " << sumTimer.GetDurationMs() << "ms\n";
	cout << '\n';
}
