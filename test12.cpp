#include "pch.h"
#include "Timer.h"
#include "OpenClInt128.h"

vector<pair<int, int>> CreateTwoCards();

void Test12()
{
	cout << "Test12: 128-bit increment in OpenCL.\n";

	vector<uint64_t> data;
	for (auto& twoCard : CreateTwoCards())
		data.push_back((1ull << twoCard.first) | (1ull << twoCard.second));

	auto device = compute::system::default_device();
	cout << "Using device: " << device.name() << '\n';
	auto context = compute::context{ device };
	auto commandQueue = compute::command_queue{ context, device };

	auto source = OpenClInt128::GetSource() + R"(
struct counts_t
{
	struct uint128_t count2, count3, count4, count5;
};
__kernel void test(__global unsigned long* data, __global struct counts_t* counts, unsigned long size)
{
	size_t id = get_global_id(0);

	unsigned long remainder = id;
	unsigned long index1 = remainder % size;
	remainder /= size;
	unsigned long index2 = remainder % size;

	if (index2 >= index1)
		return;
	__global unsigned long* i1 = data + index1;
	__global unsigned long* i2 = data + index2;
	if ((*i1 & *i2) != 0)
		return;
	unsigned long b2 = *i1 | *i2;
	__global struct counts_t* count = counts + id;

	remainder /= size;
	unsigned long index3 = remainder % size;

	if (index3 == 0)
		increment(&count->count2);
	if (index3 >= index2)
		return;
	__global unsigned long* i3 = data + index3;
	if ((b2 & *i3) != 0)
		return;
	unsigned long b3 = b2 | *i3;
	increment(&count->count3);

	for (__global unsigned long* i4 = data; i4 != i3; ++i4)
	{
		if ((b3 & *i4) != 0)
			continue;
		unsigned long b4 = b3 | *i4;
		increment(&count->count4);
		for (__global unsigned long* i5 = data; i5 != i4; ++i5)
		{
			if ((b4 & *i5) != 0)
				continue;
			increment(&count->count5);
		}
	}
}
	)";

	struct counts_t
	{
		OpenClInt128 count2, count3, count4, count5;
	};

	auto size = data.size();
	auto countsSize = size * size * size;
	vector<counts_t> counts(countsSize);
	
	Timer copyToDeviceTimer;
	compute::vector<uint64_t> deviceData(data.size(), context);
	compute::copy(data.begin(), data.end(), deviceData.begin(), commandQueue);

	compute::vector<counts_t> deviceCounts(counts.size(), context);
	compute::copy(counts.begin(), counts.end(), deviceCounts.begin(), commandQueue);
	cout << "Copy to device: " << copyToDeviceTimer.GetDurationMs() << "ms\n";

	Timer compileTimer;
	auto program = compute::program::build_with_source(source, context);
	auto kernel = program.create_kernel("test");
	kernel.set_args(deviceData.get_buffer(), deviceCounts.get_buffer(), size);
	cout << "Compilation: " << compileTimer.GetDurationMs() << "ms\n";

	Timer timer;
	commandQueue.enqueue_1d_range_kernel(kernel, 0, countsSize, 1);
	commandQueue.finish();
	cout << "Duration: " << timer.GetDurationMs() << "ms\n";

	Timer copyToHostTimer;
	compute::copy(deviceCounts.begin(), deviceCounts.end(), counts.begin(), commandQueue);
	cout << "Copy to host: " << copyToHostTimer.GetDurationMs() << "ms\n";

	Timer sumTimer;
	LargeInteger count2 = 0, count3 = 0, count4 = 0, count5 = 0;
	for (auto& count : counts)
	{
		count2 += count.count2.ToLargeInteger();
		count3 += count.count3.ToLargeInteger();
		count4 += count.count4.ToLargeInteger();
		count5 += count.count5.ToLargeInteger();
	}
	cout << "Sum: " << sumTimer.GetDurationMs() << "ms\n";

	cout << "Count2: " << count2 << '\n';
	cout << "Count3: " << count3 << '\n';
	cout << "Count4: " << count4 << '\n';
	cout << "Count5: " << count5 << '\n';
}
