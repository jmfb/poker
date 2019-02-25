#include "pch.h"
#include "Timer.h"
#include "Counts.h"

vector<pair<int, int>> CreateTwoCards();

void Test13()
{
	cout << "Test12: 128-bit increment in OpenCL - 2 loop.\n";

	vector<uint64_t> data;
	for (auto& twoCard : CreateTwoCards())
		data.push_back((1ull << twoCard.first) | (1ull << twoCard.second));

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
	++count->count2;

	for (__global unsigned long* i3 = data; i3 != i2; ++i3)
	{
		if ((b2 & *i3) != 0)
			continue;
		unsigned long b3 = b2 | *i3;
		++count->count3;
		for (__global unsigned long* i4 = data; i4 != i3; ++i4)
		{
			if ((b3 & *i4) != 0)
				continue;
			unsigned long b4 = b3 | *i4;
			++count->count4;
			for (__global unsigned long* i5 = data; i5 != i4; ++i5)
			{
				if ((b4 & *i5) != 0)
					continue;
				++count->count5;
			}
		}
	}
}
	)";

	auto size = data.size();
	auto countsSize = size * size;
	cout << "Count Size: " << countsSize << '\n';
	vector<Counts> counts(countsSize);

	Timer copyToDeviceTimer;
	compute::vector<uint64_t> deviceData(data.size(), context);
	compute::copy(data.begin(), data.end(), deviceData.begin(), commandQueue);
	cout << "Copy to device: " << copyToDeviceTimer.GetDurationMs() << "ms\n";

	compute::vector<Counts> deviceCounts(counts.size(), context);

	Timer compileTimer;
	auto program = compute::program::build_with_source(source, context);
	auto kernel = program.create_kernel("test");
	kernel.set_args(deviceData.get_buffer(), deviceCounts.get_buffer(), size);
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
