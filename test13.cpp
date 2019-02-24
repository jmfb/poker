#include "pch.h"
#include "Timer.h"
#include "OpenClCounter.h"

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

	auto source = OpenClCounter::GetSource() + R"(
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
	increment(&count->count2);

	for (__global unsigned long* i3 = data; i3 != i2; ++i3)
	{
		if ((b2 & *i3) != 0)
			continue;
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
}
	)";

	struct counts_t
	{
		OpenClCounter count2, count3, count4, count5;
		counts_t& operator+=(const counts_t& rhs)
		{
			count2 += rhs.count2;
			count3 += rhs.count3;
			count4 += rhs.count4;
			count5 += rhs.count5;
			return *this;
		}
		counts_t operator+(const counts_t& rhs) const
		{
			auto result{ *this };
			result += rhs;
			return result;
		}
	};

	auto size = data.size();
	auto countsSize = size * size;
	cout << "Count Size: " << countsSize << '\n';
	vector<counts_t> counts(countsSize);

	Timer copyToDeviceTimer;
	compute::vector<uint64_t> deviceData(data.size(), context);
	compute::copy(data.begin(), data.end(), deviceData.begin(), commandQueue);
	cout << "Copy to device: " << copyToDeviceTimer.GetDurationMs() << "ms\n";

	compute::vector<uint32_t> deviceCounts(counts.size() * 4 * 4, context);

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
	compute::copy(deviceCounts.begin(), deviceCounts.end(), reinterpret_cast<uint32_t*>(counts.data()), commandQueue);
	cout << "Copy to host: " << copyToHostTimer.GetDurationMs() << "ms\n";

	Timer sumTimer;
	auto total = reduce(execution::par_unseq, counts.begin(), counts.end(), counts_t{}, [](auto lhs, auto rhs){ return lhs + rhs; });
	cout << "Sum: " << sumTimer.GetDurationMs() << "ms\n";

	cout << "Count2: " << total.count2.Get() << '\n';
	cout << "Count3: " << total.count3.Get() << '\n';
	cout << "Count4: " << total.count4.Get() << '\n';
	cout << "Count5: " << total.count5.Get() << '\n';
	cout << '\n';
}
