#include "pch.h"
#include "OpenClInt128.h"

void Test12()
{
	cout << "Test12: 128-bit increment in OpenCL.\n";
	auto device = compute::system::default_device();
	cout << "Using device: " << device.name() << '\n';
	auto context = compute::context{ device };
	auto commandQueue = compute::command_queue{ context, device };

	auto source = OpenClInt128::GetSource() + R"(
__kernel void test(__global struct uint128_t* data)
{
	increment(&data[0]);
}
	)";

	vector<OpenClInt128> hostData(1);
	hostData[0] = OpenClInt128{ LargeInteger{ numeric_limits<uint64_t>::max() } / 2 };
	
	compute::vector<OpenClInt128> deviceData(hostData.size(), context);
	compute::copy(hostData.begin(), hostData.end(), deviceData.begin(), commandQueue);

	auto program = compute::program::build_with_source(source, context);
	auto kernel = program.create_kernel("test");
	kernel.set_args(deviceData.get_buffer());

	commandQueue.enqueue_task(kernel);
	commandQueue.finish();

	compute::copy(deviceData.begin(), deviceData.end(), hostData.begin(), commandQueue);

	cout << "Actual: " << hostData[0].ToLargeInteger() << '\n';
}
