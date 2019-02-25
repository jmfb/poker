#include "pch.h"
#include "TwoCardOverlap.h"
#include "Math.h"

TwoCardOverlap::TwoCardOverlap(const HoleCards& hole)
	: bits{ (1ull << hole.GetCard1()) | (1ull << hole.GetCard2()) }
{
}

uint64_t TwoCardOverlap::GetBits() const
{
	return bits;
}

void TwoCardOverlap::AddCount2()
{
	++counts.count2;
}

void TwoCardOverlap::AddCount3()
{
	++counts.count3;
}

void TwoCardOverlap::AddCount4()
{
	++counts.count4;
}

void TwoCardOverlap::AddCount5()
{
	++counts.count5;
}

void TwoCardOverlap::AddCount6()
{
	++counts.count6;
}

void TwoCardOverlap::AddCount7()
{
	++counts.count7;
}

void TwoCardOverlap::AddCount8()
{
	++counts.count8;
}

uint128_t TwoCardOverlap::GetTotalOverlap(uint128_t remainingCards, uint128_t opponentCards) const
{
	return counts.count2 * ComputeTotalCombinations(remainingCards, opponentCards) -
		counts.count3 * ComputeTotalCombinations(remainingCards - 2, opponentCards - 2) +
		counts.count4 * ComputeTotalCombinations(remainingCards - 4, opponentCards - 4) -
		counts.count5 * ComputeTotalCombinations(remainingCards - 6, opponentCards - 6) +
		counts.count6 * ComputeTotalCombinations(remainingCards - 8, opponentCards - 8) -
		counts.count7 * ComputeTotalCombinations(remainingCards - 10, opponentCards - 10) +
		counts.count8 * ComputeTotalCombinations(remainingCards - 12, opponentCards - 12);
}

TwoCardOverlap& TwoCardOverlap::operator+=(const TwoCardOverlap& rhs)
{
	counts += rhs.counts;
	return *this;
}

uint128_t TwoCardOverlap::Compute(const vector<HoleCards>& twoCards, int remaining, int opponents)
{
	if (opponents == 1)
		return 0;

	vector<TwoCardOverlap> overlaps;
	for (auto& twoCard : twoCards)
		overlaps.emplace_back(twoCard);
	auto begin = overlaps.data();
	auto end = begin + overlaps.size();

	switch (opponents)
	{
	case 2:
		Compute2(begin, end);
		break;
	case 3:
		Compute3(begin, end);
		break;
	case 4:
		Compute4(begin, end);
		break;
	case 5:
		Compute5(begin, end);
		break;
	case 6:
		Compute6(begin, end);
		break;
	case 7:
		Compute7(begin, end);
		break;
	case 8:
		Compute8(begin, end);
		break;
	default:
		throw runtime_error{ "Unsupported number of opponents: " + to_string(opponents) };
	}

	TwoCardOverlap total;
	for (auto& twoCard : overlaps)
		total += twoCard;
	return total.GetTotalOverlap(remaining - 4, 2 * opponents - 4);
}

void TwoCardOverlap::Compute2(TwoCardOverlap* begin, TwoCardOverlap* end)
{
	for_each(execution::par_unseq, begin, end, [&](TwoCardOverlap& i1)
	{
		for (auto i2 = begin; i2 != &i1; ++i2)
			if ((i1.GetBits() & i2->GetBits()) == 0)
				i1.AddCount2();
	});
}

void TwoCardOverlap::Compute3(TwoCardOverlap* begin, TwoCardOverlap* end)
{
	for_each(execution::par_unseq, begin, end, [&](TwoCardOverlap& i1)
	{
		for (auto i2 = begin; i2 != &i1; ++i2)
		{
			if ((i1.GetBits() & i2->GetBits()) != 0)
				continue;
			auto s2 = i1.GetBits() | i2->GetBits();
			i1.AddCount2();
			for (auto i3 = begin; i3 != i2; ++i3)
				if ((s2 & i3->GetBits()) == 0)
					i1.AddCount3();
		}
	});
}

void TwoCardOverlap::Compute4(TwoCardOverlap* begin, TwoCardOverlap* end)
{
	for_each(execution::par_unseq, begin, end, [&](TwoCardOverlap& i1)
	{
		for (auto i2 = begin; i2 != &i1; ++i2)
		{
			if ((i1.GetBits() & i2->GetBits()) != 0)
				continue;
			auto s2 = i1.GetBits() | i2->GetBits();
			i1.AddCount2();
			for (auto i3 = begin; i3 != i2; ++i3)
			{
				if ((s2 & i3->GetBits()) != 0)
					continue;
				auto s3 = s2 | i3->GetBits();
				i1.AddCount3();
				for (auto i4 = begin; i4 != i3; ++i4)
					if ((s3 & i4->GetBits()) == 0)
						i1.AddCount4();
			}
		}
	});
}

void TwoCardOverlap::Compute5(TwoCardOverlap* begin, TwoCardOverlap* end)
{
	for_each(execution::par_unseq, begin, end, [&](TwoCardOverlap& i1)
	{
		for (auto i2 = begin; i2 != &i1; ++i2)
		{
			if ((i1.GetBits() & i2->GetBits()) != 0)
				continue;
			auto s2 = i1.GetBits() | i2->GetBits();
			i1.AddCount2();
			for (auto i3 = begin; i3 != i2; ++i3)
			{
				if ((s2 & i3->GetBits()) != 0)
					continue;
				auto s3 = s2 | i3->GetBits();
				i1.AddCount3();
				for (auto i4 = begin; i4 != i3; ++i4)
				{
					if ((s3 & i4->GetBits()) != 0)
						continue;
					auto s4 = s3 | i4->GetBits();
					i1.AddCount4();
					for (auto i5 = begin; i5 != i4; ++i5)
						if ((s4 & i5->GetBits()) == 0)
							i1.AddCount5();
				}
			}
		}
	});
}

void TwoCardOverlap::Compute6(TwoCardOverlap* begin, TwoCardOverlap* end)
{
	for_each(execution::par_unseq, begin, end, [&](TwoCardOverlap& i1)
	{
		for (auto i2 = begin; i2 != &i1; ++i2)
		{
			if ((i1.GetBits() & i2->GetBits()) != 0)
				continue;
			auto s2 = i1.GetBits() | i2->GetBits();
			i1.AddCount2();
			for (auto i3 = begin; i3 != i2; ++i3)
			{
				if ((s2 & i3->GetBits()) != 0)
					continue;
				auto s3 = s2 | i3->GetBits();
				i1.AddCount3();
				for (auto i4 = begin; i4 != i3; ++i4)
				{
					if ((s3 & i4->GetBits()) != 0)
						continue;
					auto s4 = s3 | i4->GetBits();
					i1.AddCount4();
					for (auto i5 = begin; i5 != i4; ++i5)
					{
						if ((s4 & i5->GetBits()) != 0)
							continue;
						auto s5 = s4 | i5->GetBits();
						i1.AddCount5();
						for (auto i6 = begin; i6 != i5; ++i6)
							if ((s5 & i6->GetBits()) == 0)
								i1.AddCount6();
					}
				}
			}
		}
	});
}

void TwoCardOverlap::Compute7(TwoCardOverlap* begin, TwoCardOverlap* end)
{
	for_each(execution::par_unseq, begin, end, [&](TwoCardOverlap& i1)
	{
		for (auto i2 = begin; i2 != &i1; ++i2)
		{
			if ((i1.GetBits() & i2->GetBits()) != 0)
				continue;
			auto s2 = i1.GetBits() | i2->GetBits();
			i1.AddCount2();
			for (auto i3 = begin; i3 != i2; ++i3)
			{
				if ((s2 & i3->GetBits()) != 0)
					continue;
				auto s3 = s2 | i3->GetBits();
				i1.AddCount3();
				for (auto i4 = begin; i4 != i3; ++i4)
				{
					if ((s3 & i4->GetBits()) != 0)
						continue;
					auto s4 = s3 | i4->GetBits();
					i1.AddCount4();
					for (auto i5 = begin; i5 != i4; ++i5)
					{
						if ((s4 & i5->GetBits()) != 0)
							continue;
						auto s5 = s4 | i5->GetBits();
						i1.AddCount5();
						for (auto i6 = begin; i6 != i5; ++i6)
						{
							if ((s5 & i6->GetBits()) != 0)
								continue;
							auto s6 = s5 | i6->GetBits();
							i1.AddCount6();
							for (auto i7 = begin; i7 != i6; ++i7)
								if ((s6 & i7->GetBits()) == 0)
									i1.AddCount7();
						}
					}
				}
			}
		}
	});
}

void TwoCardOverlap::Compute8(TwoCardOverlap* begin, TwoCardOverlap* end)
{
	for_each(execution::par_unseq, begin, end, [&](TwoCardOverlap& i1)
	{
		for (auto i2 = begin; i2 != &i1; ++i2)
		{
			if ((i1.GetBits() & i2->GetBits()) != 0)
				continue;
			auto s2 = i1.GetBits() | i2->GetBits();
			i1.AddCount2();
			for (auto i3 = begin; i3 != i2; ++i3)
			{
				if ((s2 & i3->GetBits()) != 0)
					continue;
				auto s3 = s2 | i3->GetBits();
				i1.AddCount3();
				for (auto i4 = begin; i4 != i3; ++i4)
				{
					if ((s3 & i4->GetBits()) != 0)
						continue;
					auto s4 = s3 | i4->GetBits();
					i1.AddCount4();
					for (auto i5 = begin; i5 != i4; ++i5)
					{
						if ((s4 & i5->GetBits()) != 0)
							continue;
						auto s5 = s4 | i5->GetBits();
						i1.AddCount5();
						for (auto i6 = begin; i6 != i5; ++i6)
						{
							if ((s5 & i6->GetBits()) != 0)
								continue;
							auto s6 = s5 | i6->GetBits();
							i1.AddCount6();
							for (auto i7 = begin; i7 != i6; ++i7)
							{
								if ((s6 & i7->GetBits()) != 0)
									continue;
								auto s7 = s6 | i7->GetBits();
								i1.AddCount7();
								for (auto i8 = begin; i8 != i7; ++i8)
									if ((s7 & i8->GetBits()) == 0)
										i1.AddCount8();
							}
						}
					}
				}
			}
		}
	});
}
