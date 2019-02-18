#include "pch.h"
#include "CudaError.h"
#include "CudaMemory.h"
#include "CudaManagedMemory.h"
#include "CudaDevice.h"
#include "Timer.h"
#include "ExceptionCount.h"

#include <boost/multiprecision/cpp_int.hpp>
using LargeInteger = boost::multiprecision::int128_t;

//Laptop
//Max Threads Per Block: 1024
//Max Threads Per MultiProcessor: 1536
//MultiProcessor Count: 4
//Version: 9010

//Dekstop
//Max Threads Per Block: 1024
//Max Threads Per MultiProcessor: 2048
//MultiProcessor Count: 28
//Version: 10000
void DumpDeviceInformation()
{
	int count = 0;
	auto result = ::cudaGetDeviceCount(&count);
	CudaError::Check(__FUNCTION__, "cudaGetDeviceCount", result);

	cout << count << " devices detected.\n";
	for (auto ordinal = 0; ordinal < count; ++ordinal)
	{
		cout << "Device " << ordinal << " Attributes\n";
		cout << "Max Threads Per Block: " << CudaDevice::GetAttribute(ordinal, cudaDevAttrMaxThreadsPerBlock) << '\n';
		cout << "Max Threads Per MultiProcessor: " << CudaDevice::GetAttribute(ordinal, cudaDevAttrMaxThreadsPerMultiProcessor) << '\n';
		cout << "MultiProcessor Count: " << CudaDevice::GetAttribute(ordinal, cudaDevAttrMultiProcessorCount) << '\n';
		cout << '\n';
	}

	int driverVersion = 0;
	auto versionResult = ::cuDriverGetVersion(&driverVersion);
	CudaError::Check(__FUNCTION__, "cuDriverGetVersion", versionResult);
	cout << "Version: " << driverVersion << '\n';

	cout << "Done.\n";
}

//Host version...
//Mini-duration: 48.651ms
//Max error: 0
//Duration: 112.102ms
void TestHostOnly()
{
	const auto count = 1 << 20;
	auto x = make_unique<float[]>(count);
	auto y = make_unique<float[]>(count);

	fill(x.get(), x.get() + count, 1.0f);
	fill(y.get(), y.get() + count, 2.0f);

	cout << "Host version...\n";
	Timer miniTimer;

	for (auto index = 0; index < count; ++index)
		y.get()[index] = x.get()[index] + y.get()[index];

	cout << "Mini-duration: " << miniTimer.GetDurationMs() << "ms\n";

	float maxError = 0.0f;
	for (auto index = 0; index < count; ++index)
		maxError = max(maxError, fabs(y[index] - 3.0f));
	cout << "Max error: " << maxError << '\n';
}

void TestDevice();
void TestManaged();

void TestAdd()
{
	for (auto index = 0; index < 3; ++index)
	{
		TestHostOnly();	//Baseline non-parallel version.
		TestDevice();	//It appears using device-only memory is fastest.
		TestManaged();	//This is supported on the 1080Ti but slower than device-only with copy.
	}
}

vector<pair<int, int>> CreateTwoCards()
{
	return
	{
		{ 2, 3 }, { 2, 4 }, { 2, 5 }, { 2, 7 }, { 2, 8 }, { 2, 9 }, { 2, 10 }, { 2, 11 }, { 2, 12 },
		{ 3, 4 }, { 3, 5 }, { 3, 7 }, { 3, 8 }, { 3, 9 }, { 3, 10 }, { 3, 11 }, { 3, 12 },
		{ 4, 5 }, { 4, 7 }, { 4, 8 }, { 4, 9 }, { 4, 10 }, { 4, 11 }, { 4, 12 },
		{ 5, 7 }, { 5, 8 }, { 5, 9 }, { 5, 10 }, { 5, 11 }, { 5, 12 }, { 5, 33 }, { 5, 46 },
		{ 7, 8 }, { 7, 9 }, { 7, 10 }, { 7, 11 }, { 7, 12 }, { 7, 18 }, { 7, 23 }, { 7, 31 }, { 7, 36 }, { 7, 44 }, { 7, 49 },
		{ 8, 9 }, { 8, 10 }, { 8, 11 }, { 8, 12 }, { 8, 13 }, { 8, 14 }, { 8, 19 }, { 8, 21 },
		{ 8, 22 }, { 8, 26 }, { 8, 27 }, { 8, 32 }, { 8, 39 }, { 8, 40 }, { 8, 45 }, { 8, 47 },
		{ 9, 10 }, { 9, 11 }, { 9, 12 }, { 9, 13 }, { 9, 14 }, { 9, 19 }, { 9, 21 }, { 9, 22 },
		{ 9, 23 }, { 9, 24 }, { 9, 25 }, { 9, 26 }, { 9, 27 }, { 9, 32 }, { 9, 36 }, { 9, 37 },
		{ 9, 38 }, { 9, 39 }, { 9, 40 }, { 9, 45 }, { 9, 47 }, { 9, 49 }, { 9, 50 }, { 9, 51 },
		{ 10, 11 }, { 10, 12 }, { 10, 22 }, { 10, 23 }, { 10, 33 }, { 10, 36 }, { 10, 46 }, { 10, 49 },
		{ 11, 12 }, { 11, 22 }, { 11, 24 }, { 11, 37 }, { 11, 50 },
		{ 12, 22 }, { 12, 25 }, { 12, 38 }, { 12, 51 },
		{ 13, 14 }, { 13, 19 }, { 13, 21 }, { 13, 22 }, { 13, 26 }, { 13, 27 }, { 13, 32 }, { 13, 39 }, { 13, 40 }, { 13, 45 }, { 13, 47 },
		{ 14, 19 }, { 14, 21 }, { 14, 22 }, { 14, 26 }, { 14, 27 }, { 14, 32 }, { 14, 39 }, { 14, 40 }, { 14, 45 }, { 14, 47 },
		{ 18, 33 }, { 18, 46 },
		{ 19, 21 }, { 19, 22 }, { 19, 26 }, { 19, 27 }, { 19, 32 }, { 19, 39 }, { 19, 40 }, { 19, 45 }, { 19, 47 },
		{ 21, 22 }, { 21, 26 }, { 21, 27 }, { 21, 32 }, { 21, 39 }, { 21, 40 }, { 21, 45 }, { 21, 47 },
		{ 22, 23 }, { 22, 24 }, { 22, 25 }, { 22, 26 }, { 22, 27 }, { 22, 32 }, { 22, 36 }, { 22, 37 },
		{ 22, 38 }, { 22, 39 }, { 22, 40 }, { 22, 45 }, { 22, 47 }, { 22, 49 }, { 22, 50 }, { 22, 51 },
		{ 23, 33 }, { 23, 36 }, { 23, 46 }, { 23, 49 },
		{ 24, 37 }, { 24, 50 },
		{ 25, 38 }, { 25, 51 },
		{ 26, 27 }, { 26, 32 }, { 26, 39 }, { 26, 40 }, { 26, 45 }, { 26, 47 },
		{ 27, 32 }, { 27, 39 }, { 27, 40 }, { 27, 45 }, { 27, 47 },
		{ 31, 33 }, { 31, 46 },
		{ 32, 39 }, { 32, 40 }, { 32, 45 }, { 32, 47 },
		{ 33, 36 }, { 33, 44 }, { 33, 49 },
		{ 36, 46 }, { 36, 49 },
		{ 37, 50 },
		{ 38, 51 },
		{ 39, 40 }, { 39, 45 }, { 39, 47 },
		{ 40, 45 }, { 40, 47 },
		{ 44, 46 },
		{ 45, 47 },
		{ 46, 49 }
	};
}

LargeInteger Combinations(LargeInteger total, LargeInteger take)
{
	LargeInteger result{ 1 };
	for (LargeInteger index{ 0 }; index < take; ++index)
		result = result * (total - index) / (index + 1);
	return result;
}

LargeInteger Partitions(LargeInteger count)
{
	auto n = count / 2;
	LargeInteger product{ 1 };
	for (LargeInteger k{ 0 }; k < n; ++k)
		product *= 2 * k + 1;
	return product;
}

LargeInteger ComputeTotalCombinations(LargeInteger cards, LargeInteger opponentCards)
{
	if (opponentCards < 0)
		return 0;
	if (opponentCards == 0)
		return 1;
	return Combinations(cards, opponentCards) * Partitions(opponentCards);
}

class TwoCardOverlap
{
public:
	TwoCardOverlap() = default;
	TwoCardOverlap(const pair<int, int>& hole)
		: bits{ (1ull << hole.first) | (1ull << hole.second) }
	{
	}
	TwoCardOverlap(const TwoCardOverlap& rhs) = default;
	TwoCardOverlap(TwoCardOverlap&& rhs) = default;
	~TwoCardOverlap() = default;

	TwoCardOverlap& operator=(const TwoCardOverlap& rhs) = default;
	TwoCardOverlap& operator=(TwoCardOverlap&& rhs) = default;

	unsigned long long GetBits() const
	{
		return bits;
	}

	void AddCount2()
	{
		++count2;
	}
	void AddCount3()
	{
		++count3;
	}
	void AddCount4()
	{
		++count4;
	}
	void AddCount5()
	{
		++count5;
	}

	LargeInteger GetTotalOverlap(LargeInteger remainingCards, LargeInteger opponentCards) const
	{
		return count2 * ComputeTotalCombinations(remainingCards, opponentCards) -
			count3 * ComputeTotalCombinations(remainingCards - 2, opponentCards - 2) +
			count4 * ComputeTotalCombinations(remainingCards - 4, opponentCards - 4) -
			count5 * ComputeTotalCombinations(remainingCards - 6, opponentCards - 6);
	}

	TwoCardOverlap& operator+=(const TwoCardOverlap& rhs)
	{
		count2 += rhs.count2;
		count3 += rhs.count3;
		count4 += rhs.count4;
		count5 += rhs.count5;
		return *this;
	}

private:
	unsigned long long bits = 0;
	LargeInteger count2 = 0;
	LargeInteger count3 = 0;
	LargeInteger count4 = 0;
	LargeInteger count5 = 0;
};

void Compute5(TwoCardOverlap* begin, TwoCardOverlap* end)
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

LargeInteger Compute(const vector<pair<int, int>>& twoCards, int remaining)
{
	vector<TwoCardOverlap> overlaps;
	for (auto& twoCard : twoCards)
		overlaps.emplace_back(twoCard);
	auto begin = overlaps.data();
	auto end = begin + overlaps.size();

	Compute5(begin, end);

	TwoCardOverlap total;
	for (auto& twoCard : overlaps)
		total += twoCard;
	return total.GetTotalOverlap(remaining - 4, 2 * 5 - 4);
}

void TestOddsHost()
{
	const auto opponents = 5;
	auto twoCards = CreateTwoCards();
	auto overlap = Compute(twoCards, 45);

	auto lose = ComputeTotalCombinations(43, 2 * opponents - 2) * static_cast<long long>(twoCards.size()) - overlap;
	cout << "Lose: " << lose << " (" << opponents << " opponents)\n";
}

void TestOddsDevice();

int main()
{
	try
	{
		Timer timer;

		//Lose: 2048339780657 (5 opponents)
		//Duration: 343.818ms

		//TestOddsHost();
		TestOddsDevice();

		cout << "Duration: " << timer.GetDurationMs() << "ms\n";
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
