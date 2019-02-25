#include "pch.h"
#include "TwoCardOverlap.h"
#include "Math.h"

uint128_t TwoCardOverlap::Compute(const vector<uint64_t>& twoCards, int remaining, int opponents)
{
	if (opponents == 1)
		return 0;

	auto begin = twoCards.data();
	auto end = begin + twoCards.size();
	auto& sequence = CombinationSequences::Get(twoCards.size());
	vector<Counts> counts(sequence.size());

	switch (opponents)
	{
	case 2:
		Compute2(begin, end, counts, sequence);
		break;
	case 3:
		Compute3(begin, end, counts, sequence);
		break;
	case 4:
		Compute4(begin, end, counts, sequence);
		break;
	case 5:
		Compute5(begin, end, counts, sequence);
		break;
	case 6:
		Compute6(begin, end, counts, sequence);
		break;
	case 7:
		Compute7(begin, end, counts, sequence);
		break;
	case 8:
		Compute8(begin, end, counts, sequence);
		break;
	default:
		throw runtime_error{ "Unsupported number of opponents: " + to_string(opponents) };
	}

	return Counts::GetTotal(counts).GetTotalOverlap(remaining - 4, 2 * opponents - 4);
}

void TwoCardOverlap::Compute2(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto& p = sequence[index];
		auto i1 = begin + p.first;
		auto i2 = begin + p.second;
		if ((*i1 & *i2) == 0)
			++count.count2;
	});
}

void TwoCardOverlap::Compute3(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto& p = sequence[index];
		auto i1 = begin + p.first;
		auto i2 = begin + p.second;
		if ((*i1 & *i2) != 0)
			return;
		auto s2 = *i1 | *i2;
		++count.count2;
		for (auto i3 = begin; i3 != i2; ++i3)
			if ((s2 & *i3) == 0)
				++count.count3;
	});
}

void TwoCardOverlap::Compute4(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto& p = sequence[index];
		auto i1 = begin + p.first;
		auto i2 = begin + p.second;
		if ((*i1 & *i2) != 0)
			return;
		auto s2 = *i1 | *i2;
		++count.count2;
		for (auto i3 = begin; i3 != i2; ++i3)
		{
			if ((s2 & *i3) != 0)
				continue;
			auto s3 = s2 | *i3;
			++count.count3;
			for (auto i4 = begin; i4 != i3; ++i4)
				if ((s3 & *i4) == 0)
					++count.count4;
		}
	});
}

void TwoCardOverlap::Compute5(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto& p = sequence[index];
		auto i1 = begin + p.first;
		auto i2 = begin + p.second;
		if ((*i1 & *i2) != 0)
			return;
		auto s2 = *i1 | *i2;
		++count.count2;
		for (auto i3 = begin; i3 != i2; ++i3)
		{
			if ((s2 & *i3) != 0)
				continue;
			auto s3 = s2 | *i3;
			++count.count3;
			for (auto i4 = begin; i4 != i3; ++i4)
			{
				if ((s3 & *i4) != 0)
					continue;
				auto s4 = s3 | *i4;
				++count.count4;
				for (auto i5 = begin; i5 != i4; ++i5)
					if ((s4 & *i5) == 0)
						++count.count5;
			}
		}
	});
}

void TwoCardOverlap::Compute6(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto& p = sequence[index];
		auto i1 = begin + p.first;
		auto i2 = begin + p.second;
		if ((*i1 & *i2) != 0)
			return;
		auto s2 = *i1 | *i2;
		++count.count2;
		for (auto i3 = begin; i3 != i2; ++i3)
		{
			if ((s2 & *i3) != 0)
				continue;
			auto s3 = s2 | *i3;
			++count.count3;
			for (auto i4 = begin; i4 != i3; ++i4)
			{
				if ((s3 & *i4) != 0)
					continue;
				auto s4 = s3 | *i4;
				++count.count4;
				for (auto i5 = begin; i5 != i4; ++i5)
				{
					if ((s4 & *i5) != 0)
						continue;
					auto s5 = s4 | *i5;
					++count.count5;
					for (auto i6 = begin; i6 != i5; ++i6)
						if ((s5 & *i6) == 0)
							++count.count6;
				}
			}
		}
	});
}

void TwoCardOverlap::Compute7(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto& p = sequence[index];
		auto i1 = begin + p.first;
		auto i2 = begin + p.second;
		if ((*i1 & *i2) != 0)
			return;
		auto s2 = *i1 | *i2;
		++count.count2;
		for (auto i3 = begin; i3 != i2; ++i3)
		{
			if ((s2 & *i3) != 0)
				continue;
			auto s3 = s2 | *i3;
			++count.count3;
			for (auto i4 = begin; i4 != i3; ++i4)
			{
				if ((s3 & *i4) != 0)
					continue;
				auto s4 = s3 | *i4;
				++count.count4;
				for (auto i5 = begin; i5 != i4; ++i5)
				{
					if ((s4 & *i5) != 0)
						continue;
					auto s5 = s4 | *i5;
					++count.count5;
					for (auto i6 = begin; i6 != i5; ++i6)
					{
						if ((s5 & *i6) != 0)
							continue;
						auto s6 = s5 | *i6;
						++count.count6;
						for (auto i7 = begin; i7 != i6; ++i7)
							if ((s6 & *i7) == 0)
								++count.count7;
					}
				}
			}
		}
	});
}

void TwoCardOverlap::Compute8(const uint64_t* begin, const uint64_t* end, vector<Counts>& counts, const CombinationSequence& sequence)
{
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto& p = sequence[index];
		auto i1 = begin + p.first;
		auto i2 = begin + p.second;
		if ((*i1 & *i2) != 0)
			return;
		auto s2 = *i1 | *i2;
		++count.count2;
		for (auto i3 = begin; i3 != i2; ++i3)
		{
			if ((s2 & *i3) != 0)
				continue;
			auto s3 = s2 | *i3;
			++count.count3;
			for (auto i4 = begin; i4 != i3; ++i4)
			{
				if ((s3 & *i4) != 0)
					continue;
				auto s4 = s3 | *i4;
				++count.count4;
				for (auto i5 = begin; i5 != i4; ++i5)
				{
					if ((s4 & *i5) != 0)
						continue;
					auto s5 = s4 | *i5;
					++count.count5;
					for (auto i6 = begin; i6 != i5; ++i6)
					{
						if ((s5 & *i6) != 0)
							continue;
						auto s6 = s5 | *i6;
						++count.count6;
						for (auto i7 = begin; i7 != i6; ++i7)
						{
							if ((s6 & *i7) != 0)
								continue;
							auto s7 = s6 | *i7;
							++count.count7;
							for (auto i8 = begin; i8 != i7; ++i8)
								if ((s7 & *i8) == 0)
									++count.count8;
						}
					}
				}
			}
		}
	});
}
