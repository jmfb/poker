#include "pch.h"
#include "TwoCardOverlap.h"
#include "Math.h"

uint128_t TwoCardOverlap::Compute(const vector<uint64_t>& twoCards, int remaining, int opponents)
{
	if (opponents == 1 || twoCards.empty())
		return 0;

	vector<Bitset> m(twoCards.size());
	for (auto row = 0; row < (twoCards.size() - 1); ++row)
		for (auto column = row + 1; column < twoCards.size(); ++column)
			if ((twoCards[row] & twoCards[column]) == 0)
				m[row].Set(column);

	vector<Counts> counts(opponents < 4 ? m.size() - 1 : m.size() * m.size());
	switch (opponents)
	{
	case 2:
		Compute2(m, counts);
		break;
	case 3:
		Compute3(m, counts);
		break;
	case 4:
		Compute4(m, counts);
		break;
	case 5:
		Compute5(m, counts);
		break;
	case 6:
		Compute6(m, counts);
		break;
	case 7:
		Compute7(m, counts);
		break;
	case 8:
		Compute8(m, counts);
		break;
	default:
		throw runtime_error{ "Unsupported number of opponents: " + to_string(opponents) };
	}

	return Counts::GetTotal(counts).GetTotalOverlap(remaining - 4, 2 * opponents - 4);
}

void TwoCardOverlap::Compute2(const vector<Bitset>& m, vector<Counts>& counts)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(m.size() - 1), [&](uint64_t index)
	{
		counts[index].count2 = static_cast<uint32_t>(m[index].Count());
	});
}

void TwoCardOverlap::Compute3(const vector<Bitset>& m, vector<Counts>& counts)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(m.size() - 1), [&](uint64_t index)
	{
		auto i1 = static_cast<uint32_t>(index);
		auto& count = counts[i1];
		auto r1 = m[i1];
		auto c2 = r1.Count();
		if (c2 == 0)
			return;
		count.count2 += static_cast<uint32_t>(c2);
		for (auto i2 = i1; c2--; )
		{
			i2 = r1.CountLeadingZeros(i2 + 1);
			r1.Set(i2, false);
			count.count3 += static_cast<uint32_t>((r1 & m[i2]).Count());
		}
	});
}

void TwoCardOverlap::Compute4(const vector<Bitset>& m, vector<Counts>& counts)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(m.size() * m.size()), [&](uint64_t index)
	{
		auto i1 = static_cast<uint32_t>(index % m.size());
		auto i2 = static_cast<uint32_t>(index / m.size());
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
			count.count4 += static_cast<uint32_t>((r2 & m[i3]).Count());
		}
	});
}

void TwoCardOverlap::Compute5(const vector<Bitset>& m, vector<Counts>& counts)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(m.size() * m.size()), [&](uint64_t index)
	{
		auto i1 = static_cast<uint32_t>(index % m.size());
		auto i2 = static_cast<uint32_t>(index / m.size());
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
}

void TwoCardOverlap::Compute6(const vector<Bitset>& m, vector<Counts>& counts)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(m.size() * m.size()), [&](uint64_t index)
	{
		auto i1 = static_cast<uint32_t>(index % m.size());
		auto i2 = static_cast<uint32_t>(index / m.size());
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
				auto r4 = r3 & m[i4];
				auto c5 = r4.Count();
				if (c5 == 0)
					continue;
				count.count5 += c5;
				for (auto i5 = i4; c5--; )
				{
					i5 = r4.CountLeadingZeros(i5 + 1);
					r4.Set(i5, false);
					count.count6 += (r4 & m[i5]).Count();
				}
			}
		}
	});
}

void TwoCardOverlap::Compute7(const vector<Bitset>& m, vector<Counts>& counts)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(m.size() * m.size()), [&](uint64_t index)
	{
		auto i1 = static_cast<uint32_t>(index % m.size());
		auto i2 = static_cast<uint32_t>(index / m.size());
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
				auto r4 = r3 & m[i4];
				auto c5 = r4.Count();
				if (c5 == 0)
					continue;
				count.count5 += c5;
				for (auto i5 = i4; c5--; )
				{
					i5 = r4.CountLeadingZeros(i5 + 1);
					r4.Set(i5, false);
					auto r5 = r4 & m[i5];
					auto c6 = r5.Count();
					if (c6 == 0)
						continue;
					count.count6 += c6;
					for (auto i6 = i5; c6--; )
					{
						i6 = r5.CountLeadingZeros(i6 + 1);
						r5.Set(i6, false);
						count.count7 += (r5 & m[i6]).Count();
					}
				}
			}
		}
	});
}

void TwoCardOverlap::Compute8(const vector<Bitset>& m, vector<Counts>& counts)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(m.size() * m.size()), [&](uint64_t index)
	{
		auto i1 = static_cast<uint32_t>(index % m.size());
		auto i2 = static_cast<uint32_t>(index / m.size());
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
				auto r4 = r3 & m[i4];
				auto c5 = r4.Count();
				if (c5 == 0)
					continue;
				count.count5 += c5;
				for (auto i5 = i4; c5--; )
				{
					i5 = r4.CountLeadingZeros(i5 + 1);
					r4.Set(i5, false);
					auto r5 = r4 & m[i5];
					auto c6 = r5.Count();
					if (c6 == 0)
						continue;
					count.count6 += c6;
					for (auto i6 = i5; c6--; )
					{
						i6 = r5.CountLeadingZeros(i6 + 1);
						r5.Set(i6, false);
						auto r6 = r5 & m[i6];
						auto c7 = r6.Count();
						if (c7 == 0)
							continue;
						count.count7 += c7;
						for (auto i7 = i6; c7--; )
						{
							i7 = r6.CountLeadingZeros(i7 + 1);
							r6.Set(i7, false);
							count.count8 += (r6 & m[i7]).Count();
						}
					}
				}
			}
		}
	});
}
