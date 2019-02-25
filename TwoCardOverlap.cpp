#include "pch.h"
#include "TwoCardOverlap.h"
#include "Math.h"

uint128_t TwoCardOverlap::Compute(const vector<uint64_t>& twoCards, int remaining, int opponents)
{
	if (opponents == 1)
		return 0;

	auto begin = twoCards.data();
	auto end = begin + twoCards.size();
	Counts counts;

	switch (opponents)
	{
	case 2:
		counts = Compute2(begin, end);
		break;
	case 3:
		counts = Compute3(begin, end);
		break;
	case 4:
		counts = Compute4(begin, end);
		break;
	case 5:
		counts = Compute5(begin, end);
		break;
	case 6:
		counts = Compute6(begin, end);
		break;
	case 7:
		counts = Compute7(begin, end);
		break;
	case 8:
		counts = Compute8(begin, end);
		break;
	default:
		throw runtime_error{ "Unsupported number of opponents: " + to_string(opponents) };
	}

	return counts.GetTotalOverlap(remaining - 4, 2 * opponents - 4);
}

Counts TwoCardOverlap::Compute2(const uint64_t* begin, const uint64_t* end)
{
	vector<Counts> counts(end - begin);
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto i1 = begin + index;
		for (auto i2 = begin; i2 != i1; ++i2)
			if ((*i1 & *i2) == 0)
				++count.count2;
	});
	return Counts::GetTotal(counts);
}

Counts TwoCardOverlap::Compute3(const uint64_t* begin, const uint64_t* end)
{
	vector<Counts> counts(end - begin);
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto i1 = begin + index;
		for (auto i2 = begin; i2 != i1; ++i2)
		{
			if ((*i1 & *i2) != 0)
				continue;
			auto s2 = *i1 | *i2;
			++count.count2;
			for (auto i3 = begin; i3 != i2; ++i3)
				if ((s2 & *i3) == 0)
					++count.count3;
		}
	});
	return Counts::GetTotal(counts);
}

Counts TwoCardOverlap::Compute4(const uint64_t* begin, const uint64_t* end)
{
	vector<Counts> counts(end - begin);
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto i1 = begin + index;
		for (auto i2 = begin; i2 != i1; ++i2)
		{
			if ((*i1 & *i2) != 0)
				continue;
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
		}
	});
	return Counts::GetTotal(counts);
}

Counts TwoCardOverlap::Compute5(const uint64_t* begin, const uint64_t* end)
{
	vector<Counts> counts(end - begin);
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto i1 = begin + index;
		for (auto i2 = begin; i2 != i1; ++i2)
		{
			if ((*i1 & *i2) != 0)
				continue;
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
		}
	});
	return Counts::GetTotal(counts);
}

Counts TwoCardOverlap::Compute6(const uint64_t* begin, const uint64_t* end)
{
	vector<Counts> counts(end - begin);
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto i1 = begin + index;
		for (auto i2 = begin; i2 != i1; ++i2)
		{
			if ((*i1 & *i2) != 0)
				continue;
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
		}
	});
	return Counts::GetTotal(counts);
}

Counts TwoCardOverlap::Compute7(const uint64_t* begin, const uint64_t* end)
{
	vector<Counts> counts(end - begin);
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto i1 = begin + index;
		for (auto i2 = begin; i2 != i1; ++i2)
		{
			if ((*i1 & *i2) != 0)
				continue;
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
		}
	});
	return Counts::GetTotal(counts);
}

Counts TwoCardOverlap::Compute8(const uint64_t* begin, const uint64_t* end)
{
	vector<Counts> counts(end - begin);
	for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(counts.size()), [&](uint64_t index)
	{
		auto& count = counts[index];
		auto i1 = begin + index;
		for (auto i2 = begin; i2 != i1; ++i2)
		{
			if ((*i1 & *i2) != 0)
				continue;
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
		}
	});
	return Counts::GetTotal(counts);
}
