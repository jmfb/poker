#include "pch.h"
#include "Timer.h"
#include "Counts.h"
#include "Bitset.h"

vector<pair<int, int>> CreateTwoCards();

void Test18()
{
	cout << "Test18: Attempting disjoint graph solution via GPU and 3-loop parallelization.\n";

	auto device = compute::system::default_device();
	cout << "Using device: " << device.name() << '\n';
	auto context = compute::context{ device };
	auto commandQueue = compute::command_queue{ context, device };

	auto source = R"(
struct counts_t
{
	unsigned int count2, count3, count4;
	unsigned long count5, count6, count7, count8;
};
struct bitset_t
{
	unsigned int data0;
	unsigned int data1;
	unsigned int data2;
	unsigned int data3;
	unsigned int data4;
	unsigned int data5;
	unsigned int data6;
	unsigned int data7;
	unsigned int data8;
	unsigned int data9;
};
unsigned int BitCount32(unsigned int value)
{
	asm("popc.b32 %0,%0;" : "+r" (value));
	return value;
}
unsigned int CountLeadingZeros32(unsigned int value)
{
	asm("brev.b32 %0,%0; clz.b32 %0,%0;" : "+r" (value));
	return value;
}
unsigned int BitCountGlobal(__global struct bitset_t* b)
{
	unsigned int count = 0;
	count += BitCount32(b->data0);
	count += BitCount32(b->data1);
	count += BitCount32(b->data2);
	count += BitCount32(b->data3);
	count += BitCount32(b->data4);
	count += BitCount32(b->data5);
	count += BitCount32(b->data6);
	count += BitCount32(b->data7);
	count += BitCount32(b->data8);
	count += BitCount32(b->data9);
	return count;
}
unsigned int BitCountPrivate(__private struct bitset_t* b)
{
	unsigned int count = 0;
	count += BitCount32(b->data0);
	count += BitCount32(b->data1);
	count += BitCount32(b->data2);
	count += BitCount32(b->data3);
	count += BitCount32(b->data4);
	count += BitCount32(b->data5);
	count += BitCount32(b->data6);
	count += BitCount32(b->data7);
	count += BitCount32(b->data8);
	count += BitCount32(b->data9);
	return count;
}
unsigned int CountLeadingZeros(struct bitset_t* b, unsigned int startAt)
{
	switch (startAt / 32)
	{
	case 0:
		if (b->data0 != 0)
			return CountLeadingZeros32(b->data0);
	case 1:
		if (b->data1 != 0)
			return CountLeadingZeros32(b->data1) + 32;
	case 2:
		if (b->data2 != 0)
			return CountLeadingZeros32(b->data2) + 64;
	case 3:
		if (b->data3 != 0)
			return CountLeadingZeros32(b->data3) + 96;
	case 4:
		if (b->data4 != 0)
			return CountLeadingZeros32(b->data4) + 128;
	case 5:
		if (b->data5 != 0)
			return CountLeadingZeros32(b->data5) + 160;
	case 6:
		if (b->data6 != 0)
			return CountLeadingZeros32(b->data6) + 192;
	case 7:
		if (b->data7 != 0)
			return CountLeadingZeros32(b->data7) + 224;
	case 8:
		if (b->data8 != 0)
			return CountLeadingZeros32(b->data8) + 256;
	case 9:
		if (b->data9 != 0)
			return CountLeadingZeros32(b->data9) + 288;
	}
	return 320;
}
bool IsSetPrivate(__private struct bitset_t* b, unsigned int index)
{
	unsigned int mask = 1 << (index % 32);
	switch (index / 32)
	{
	case 0:
		return (b->data0 & mask) != 0;
	case 1:
		return (b->data1 & mask) != 0;
	case 2:
		return (b->data2 & mask) != 0;
	case 3:
		return (b->data3 & mask) != 0;
	case 4:
		return (b->data4 & mask) != 0;
	case 5:
		return (b->data5 & mask) != 0;
	case 6:
		return (b->data6 & mask) != 0;
	case 7:
		return (b->data7 & mask) != 0;
	case 8:
		return (b->data8 & mask) != 0;
	case 9:
		return (b->data9 & mask) != 0;
	}
	return false;
}
bool IsSetGlobal(__global struct bitset_t* b, unsigned int index)
{
	unsigned int mask = 1 << (index % 32);
	switch (index / 32)
	{
	case 0:
		return (b->data0 & mask) != 0;
	case 1:
		return (b->data1 & mask) != 0;
	case 2:
		return (b->data2 & mask) != 0;
	case 3:
		return (b->data3 & mask) != 0;
	case 4:
		return (b->data4 & mask) != 0;
	case 5:
		return (b->data5 & mask) != 0;
	case 6:
		return (b->data6 & mask) != 0;
	case 7:
		return (b->data7 & mask) != 0;
	case 8:
		return (b->data8 & mask) != 0;
	case 9:
		return (b->data9 & mask) != 0;
	}
	return false;
}
void AndGlobal(__private struct bitset_t* dest, __global struct bitset_t* lhs, __global struct bitset_t* rhs)
{
	dest->data0 = lhs->data0 & rhs->data0;
	dest->data1 = lhs->data1 & rhs->data1;
	dest->data2 = lhs->data2 & rhs->data2;
	dest->data3 = lhs->data3 & rhs->data3;
	dest->data4 = lhs->data4 & rhs->data4;
	dest->data5 = lhs->data5 & rhs->data5;
	dest->data6 = lhs->data6 & rhs->data6;
	dest->data7 = lhs->data7 & rhs->data7;
	dest->data8 = lhs->data8 & rhs->data8;
	dest->data9 = lhs->data9 & rhs->data9;
}
void AndPrivate(__private struct bitset_t* dest, __private struct bitset_t* lhs, __global struct bitset_t* rhs)
{
	dest->data0 = lhs->data0 & rhs->data0;
	dest->data1 = lhs->data1 & rhs->data1;
	dest->data2 = lhs->data2 & rhs->data2;
	dest->data3 = lhs->data3 & rhs->data3;
	dest->data4 = lhs->data4 & rhs->data4;
	dest->data5 = lhs->data5 & rhs->data5;
	dest->data6 = lhs->data6 & rhs->data6;
	dest->data7 = lhs->data7 & rhs->data7;
	dest->data8 = lhs->data8 & rhs->data8;
	dest->data9 = lhs->data9 & rhs->data9;
}
void Clear(__private struct bitset_t* b, unsigned int index)
{
	unsigned int mask = ~(1 << index);
	switch (index / 32)
	{
	case 0:
		b->data0 &= mask;
		return;
	case 1:
		b->data1 &= mask;
		return;
	case 2:
		b->data2 &= mask;
		return;
	case 3:
		b->data3 &= mask;
		return;
	case 4:
		b->data4 &= mask;
		return;
	case 5:
		b->data5 &= mask;
		return;
	case 6:
		b->data6 &= mask;
		return;
	case 7:
		b->data7 &= mask;
		return;
	case 8:
		b->data8 &= mask;
		return;
	case 9:
		b->data9 &= mask;
		return;
	}
}
__kernel void test(__global struct bitset_t* m, __global struct counts_t* counts, unsigned long size)
{
	size_t id = get_global_id(0);

	unsigned long remainder = id;
	unsigned int i1 = remainder % size;
	remainder /= size;
	unsigned int i2 = remainder % size;
	remainder /= size;
	unsigned int i3 = remainder % size;
	__global struct counts_t* count = counts + id;
	__global struct bitset_t* r1 = m + i1;

	if (i2 == 0 && i3 == 0)
		count->count2 += BitCountGlobal(r1);
	if (!IsSetGlobal(r1, i2))
		return;

	struct bitset_t r2;
	AndGlobal(&r2, r1, m + i2);
	if (i3 == 0)
		count->count3 += BitCountPrivate(&r2);
	if (!IsSetPrivate(&r2, i3))
		return;

	struct bitset_t r3;
	AndPrivate(&r3, &r2, m + i3);
	unsigned int c4 = BitCountPrivate(&r3);
	if (c4 == 0)
		return;
	count->count4 += c4;
	for (unsigned long i4 = i3; c4--; )
	{
		i4 = CountLeadingZeros(&r3, i4 + 1);
		Clear(&r3, i4);
		struct bitset_t r4;
		AndPrivate(&r4, &r3, m + i4);
		count->count5 += BitCountPrivate(&r4);
	}
}
	)";

	vector<uint64_t> data;
	for (auto& twoCard : CreateTwoCards())
		data.push_back((1ull << twoCard.first) | (1ull << twoCard.second));

	vector<Bitset> m(data.size());
	for (auto x = 0; x < (data.size() - 1); ++x)
		for (auto y = x + 1; y < data.size(); ++y)
			if ((data[x] & data[y]) == 0)
				m[x].Set(y);

	auto size = m.size();
	auto countsSize = size * size * size;
	cout << "Count Size: " << countsSize << '\n';
	vector<Counts> counts(countsSize);

	Timer copyToDeviceTimer;
	compute::vector<Bitset> deviceMatrix(m.size(), context);
	compute::copy(m.begin(), m.end(), deviceMatrix.begin(), commandQueue);
	cout << "Copy to device: " << copyToDeviceTimer.GetDurationMs() << "ms\n";

	compute::vector<Counts> deviceCounts(counts.size(), context);

	Timer compileTimer;
	auto program = compute::program::build_with_source(source, context);
	auto kernel = program.create_kernel("test");
	kernel.set_args(deviceMatrix.get_buffer(), deviceCounts.get_buffer(), size);
	cout << "Compilation: " << compileTimer.GetDurationMs() << "ms\n";

	Timer timer;
	commandQueue.enqueue_1d_range_kernel(kernel, 0, countsSize, 1);
	commandQueue.finish();
	cout << "Execution: " << timer.GetDurationMs() << "ms\n";

	Timer copyToHostTimer;
	compute::copy(deviceCounts.begin(), deviceCounts.end(), counts.begin(), commandQueue);
	cout << "Copy to host: " << copyToHostTimer.GetDurationMs() << "ms\n";

	Timer sumTimer;
	Counts::GetTotal(counts).Dump();
	cout << "Sum: " << sumTimer.GetDurationMs() << "ms\n";
	cout << '\n';
}
