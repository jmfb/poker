#include "pch.h"
#include "Timer.h"
#include "Counts.h"
#include "Bitset.h"

vector<pair<int, int>> CreateTwoCards();

void Test16()
{
	cout << "Test16: Clique solution with 2-loop parallelization.\n";

	Timer timer;

	vector<uint64_t> data;
	for (auto& twoCard : CreateTwoCards())
		data.push_back((1ull << twoCard.first) | (1ull << twoCard.second));

	vector<Bitset> m(data.size());
	for (auto x = 0; x < (data.size() - 1); ++x)
		for (auto y = x + 1; y < data.size(); ++y)
			if ((data[x] & data[y]) == 0)
				m[x].Set(y);

	auto size = data.size();
	auto countsSize = size * size;
	vector<Counts> counts(countsSize);
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(countsSize), [&](uint64_t index)
	{
		auto i1 = static_cast<uint32_t>(index % size);
		auto i2 = static_cast<uint32_t>(index / size);
		if (i2 <= i1)
			return;
		auto& count = counts[index];
		auto& r1 = m[i1];
		if (!r1[i2])
			return;
		++count.count2;
		auto r2 = r1 & m[i2];
		auto c3 = r2.Count();
		if (c3 == 0)
			return;
		count.count3 += static_cast<uint32_t>(c3);
		for (auto i3 = i2; c3--; )
		{
			i3 = r2.CountLeadingZeros(i3 + 1);
			r2.Set(i3, false);
			auto r3 = r2 & m[i3];
			auto c4 = r3.Count();
			if (c4 == 0)
				continue;
			count.count4 += static_cast<uint32_t>(c4);
			for (auto i4 = i3; c4--; )
			{
				i4 = r3.CountLeadingZeros(i4 + 1);
				r3.Set(i4, false);
				count.count5 += (r3 & m[i4]).Count();
			}
		}
	});

	cout << "Duration: " << timer.GetDurationMs() << "ms\n";
	cout << "Count1: " << m.size() << '\n';
	Counts::GetTotal(counts).Dump();
}
