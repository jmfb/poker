#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <execution>
#include <memory>
using namespace std;
using namespace chrono;

#include <boost/compute.hpp>
namespace compute = boost::compute;

#include <boost/multiprecision/cpp_int.hpp>
using LargeInteger = boost::multiprecision::int128_t;

#include <CL/cl.h>

#pragma comment(lib, "OpenCL.lib")

#include "int128.h"

class Timer
{
public:
	Timer()
		: start{ system_clock::now() }
	{
	}

	double GetDurationMs() const
	{
		return duration_cast<microseconds>(system_clock::now() - start).count() / 1000.0;
	}

private:
	time_point<system_clock> start;
};

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

using bits_t = unsigned long long;
using count_t = unsigned long long;

class Overlap
{
public:
	Overlap()
	{
		fill(count2, count2 + 990, 0);
		fill(count3, count3 + 990, 0);
		fill(count4, count4 + 990, 0);
		fill(count5, count5 + 990, 0);
	}
	Overlap(int card1, int card2)
		: Overlap{}
	{
		bits = (1ull << card1) | (1ull << card2);
	}

	bits_t bits = 0;
	count_t count2[990];
	count_t count3[990];
	count_t count4[990];
	count_t count5[990];

	Overlap& operator+=(const Overlap& rhs)
	{
		count2[0] += accumulate(rhs.count2, rhs.count2 + 990, 0ull);
		count3[0] += accumulate(rhs.count3, rhs.count3 + 990, 0ull);
		count4[0] += accumulate(rhs.count4, rhs.count4 + 990, 0ull);
		count5[0] += accumulate(rhs.count5, rhs.count5 + 990, 0ull);
		return *this;
	}

	LargeInteger GetTotalOverlap(LargeInteger remainingCards, LargeInteger opponentCards) const
	{
		return count2[0] * ComputeTotalCombinations(remainingCards, opponentCards) -
			count3[0] * ComputeTotalCombinations(remainingCards - 2, opponentCards - 2) +
			count4[0] * ComputeTotalCombinations(remainingCards - 4, opponentCards - 4) -
			count5[0] * ComputeTotalCombinations(remainingCards - 6, opponentCards - 6);
	}
};

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

vector<Overlap> CreateOverlaps()
{
	auto twoCards = CreateTwoCards();
	vector<Overlap> overlaps;
	for (auto& twoCard : twoCards)
		overlaps.emplace_back(twoCard.first, twoCard.second);
	return overlaps;
}

const char* sourceA = R"(
//NOTE: int = 4 bytes, long = 8 bytes, long long = 16 bytes!
typedef unsigned long bits_t;
typedef unsigned long count_t;

struct Overlap
{
	bits_t bits;
	count_t count2;
	count_t count3;
	count_t count4;
	count_t count5;
};

__kernel void test(__global struct Overlap* data, int size)
{
	int id = get_global_id(0);
	if (id < size)
	{
		__global struct Overlap* i1 = data + id;
		for (__global struct Overlap* i2 = data; i2 != i1; ++i2)
		{
			if ((i1->bits & i2->bits) != 0)
				continue;
			bits_t s2 = i1->bits | i2->bits;
			++i1->count2;
			for (__global struct Overlap* i3 = data; i3 != i2; ++i3)
			{
				if ((s2 & i3->bits) != 0)
					continue;
				bits_t s3 = s2 | i3->bits;
				++i1->count3;
				for (__global struct Overlap* i4 = data; i4 != i3; ++i4)
				{
					if ((s3 & i4->bits) != 0)
						continue;
					bits_t s4 = s3 | i4->bits;
					++i1->count4;
					//Inner 5th loop causes invalid command queue most likely due
					//to exceeding OS limit for video card response times.
					//for (__global struct Overlap* i5 = data; i5 != i4; ++i5)
					//{
					//	if ((s4 & i5->bits) != 0)
					//		continue;
					//	++i1->count5;
					//}
				}
			}
		}
	}
}
)";

const char* sourceB = R"(
//#pragma OPENCL EXTENSION cl_khr_int64_base_atomics : enable

typedef unsigned long bits_t;
typedef unsigned long count_t;

struct Overlap
{
	bits_t bits;
	count_t count2[990];
	count_t count3[990];
	count_t count4[990];
	count_t count5[990];
};

__kernel void test(__global struct Overlap* data, int size)
{
	int id = get_global_id(0);
	int index = id % 990;
	int index1 = (id / size) % size;
	int index2 = id % size;
	if (index2 >= index1)
		return;
	__global struct Overlap* i1 = data + index1;
	__global struct Overlap* i2 = data + index2;
	if ((i1->bits & i2->bits) != 0)
		return;
	bits_t s2 = i1->bits | i2->bits;
	++i1->count2[index];
	for (__global struct Overlap* i3 = data; i3 != i2; ++i3)
	{
		if ((s2 & i3->bits) != 0)
			continue;
		bits_t s3 = s2 | i3->bits;
		++i1->count3[index];
		for (__global struct Overlap* i4 = data; i4 != i3; ++i4)
		{
			if ((s3 & i4->bits) != 0)
				continue;
			bits_t s4 = s3 | i4->bits;
			++i1->count4[index];
			for (__global struct Overlap* i5 = data; i5 != i4; ++i5)
			{
				if ((s4 & i5->bits) != 0)
					continue;
				++i1->count5[index];
			}
		}
	}
}
)";

const char* sourceC = R"(
typedef unsigned long bits_t;
typedef unsigned long count_t;

struct Overlap
{
	bits_t bits;
	count_t count2[990];
	count_t count3[990];
	count_t count4[990];
	count_t count5[990];
};

__kernel void test(__global struct Overlap* data, int size)
{
	int index1 = get_global_id(0);
	int index2 = get_global_id(1);
	int id = index1 * size + index2;
	int index = id % 990;
	if (index2 >= index1)
		return;
	__global struct Overlap* i1 = data + index1;
	__global struct Overlap* i2 = data + index2;
	if ((i1->bits & i2->bits) != 0)
		return;
	bits_t s2 = i1->bits | i2->bits;
	++i1->count2[index];
	for (__global struct Overlap* i3 = data; i3 != i2; ++i3)
	{
		if ((s2 & i3->bits) != 0)
			continue;
		bits_t s3 = s2 | i3->bits;
		++i1->count3[index];
		for (__global struct Overlap* i4 = data; i4 != i3; ++i4)
		{
			if ((s3 & i4->bits) != 0)
				continue;
			bits_t s4 = s3 | i4->bits;
			++i1->count4[index];
			for (__global struct Overlap* i5 = data; i5 != i4; ++i5)
			{
				if ((s4 & i5->bits) != 0)
					continue;
				++i1->count5[index];
			}
		}
	}
}
)";

int main()
{
	try
	{
		//Lose: 2048339780657 (5 opponents)
		//Duration: 2170.98ms (host, single threaded)
		//Duration: 488.725ms (host, for_each(par_unseq))

		//Using sourceA
		//Counts: 16569 800361 24670043 0
		//Overlap: 936448562385
		//Lose: 2047826635155
		//Duration: 548.37ms

		//Using sourceB with atomic_inc
		//Counts: 16569 800361 24670043 513145502
		//Overlap: 935935416883
		//Lose: 2048339780657
		//Duration: 2153.51ms

		//Using sourceB with size * size workload to unwrap top two loops
		//NOTE: race condition in countX[...] arrays.
		//DurationA: 719.232ms
		//Counts: 16569 800361 24670043 513145502
		//Overlap: 935935416883
		//Lose: 2048339780657
		//DurationB: 722.456ms

		auto hostData = CreateOverlaps();

		auto device = compute::system::default_device();
		compute::context deviceContext{ device };
		compute::command_queue commandQueue{ deviceContext, device };

		compute::vector<Overlap> deviceData{ hostData.size(), deviceContext };
		compute::copy(hostData.begin(), hostData.end(), deviceData.begin(), commandQueue);

		auto program = compute::program::build_with_source(sourceC, deviceContext);
		auto kernel = program.create_kernel("test");

		kernel.set_args(deviceData.get_buffer(), static_cast<int>(deviceData.size()));

		Timer timer;

		commandQueue.enqueue_nd_range_kernel<2>(kernel, { 0, 0 }, { deviceData.size(), deviceData.size() }, { 1, 1 });
		commandQueue.finish();

		compute::copy(deviceData.begin(), deviceData.end(), hostData.begin(), commandQueue);

		cout << "DurationA: " << timer.GetDurationMs() << "ms\n";

		Overlap total;
		for (auto& x : hostData)
			total += x;

		auto overlap = total.GetTotalOverlap(41, 6);
		auto lose = ComputeTotalCombinations(43, 8) * static_cast<long long>(hostData.size()) - overlap;

		cout << "Counts: " << total.count2[0] << ' ' << total.count3[0] << ' ' << total.count4[0] << ' ' << total.count5[0] << '\n';
		cout << "Overlap: " << overlap << '\n';
		cout << "Lose: " << lose << '\n';
		cout << "DurationB: " << timer.GetDurationMs() << "ms\n";
	}
	catch (const compute::program_build_failure& failure)
	{
		cout << failure.build_log() << '\n';
		return -1;
	}
	catch (const compute::opencl_error& error)
	{
		cout << error.error_code() << ": " << error.error_string() << '\n';
		return -1;
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}

	return 0;
}
