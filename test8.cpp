#include "pch.h"
#include "Timer.h"

vector<pair<int, int>> CreateTwoCards();

void Test8()
{
	cout << "Test8\n";
	//Execute duration: 3689.61ms
	//Size: 196
	//Count2: 16569
	//Count3: 800361
	//Count4: 24670043
	//Count5: 513145502
	//Duration: 3932.67ms
	//Sum Duration: 16.744ms

	Timer timer;
	auto twoCards = CreateTwoCards();

	struct row_t
	{
		int value;
		int colCount;
		int columns[52];
	};
	using matrix_t = vector<row_t>;
	map<int, int> rowIndexByValue;
	matrix_t m;
	for (auto& twoCard : twoCards)
	{
		auto iter = rowIndexByValue.find(twoCard.first);
		if (iter == rowIndexByValue.end())
		{
			rowIndexByValue.emplace(twoCard.first, static_cast<int>(m.size()));
			m.push_back({ twoCard.first, 1 });
			m.back().columns[0] = twoCard.second;
		}
		else
			m[iter->second].columns[m[iter->second].colCount++] = twoCard.second;
	}
	sort(execution::par_unseq, m.begin(), m.end(), [](const row_t& lhs, const row_t& rhs) -> bool
	{
		return (lhs.colCount > rhs.colCount) ||
			(lhs.colCount == rhs.colCount && lhs.value < rhs.value);
	});

	auto rowSize = static_cast<int>(m.size());
	auto colSize = m[0].colCount;
	auto countSize = rowSize * rowSize * colSize * colSize;
	struct counts_t
	{
		unsigned long long count2 = 0, count3 = 0, count4 = 0, count5 = 0;
	};
	vector<counts_t> counts(countSize);

	const char* source = R"(
struct row_t
{
	int value;
	int colCount;
	int columns[52];
};
struct counts_t
{
	unsigned long count2, count3, count4, count5;
};
__kernel void test(__global struct row_t* m, __global struct counts_t* c, int rowSize, int colSize)
{
	int id = get_global_id(0);
	int remainder = id;
	int row1 = remainder % rowSize;
	remainder /= rowSize;
	int row2 = remainder % rowSize;
	remainder /= rowSize;
	int colIndex1 = remainder % colSize;
	remainder /= colSize;
	int colIndex2 = remainder % colSize;
	if (row2 <= row1)
		return;
	__global struct row_t* r1 = m + row1;
	__global struct row_t* r2 = m + row2;
	if (colIndex1 >= r1->colCount)
		return;
	if (colIndex2 >= r2->colCount)
		return;
	int col1 = r1->columns[colIndex1];
	int col2 = r2->columns[colIndex2];
	unsigned long b1 = (1ul << r1->value) | (1ul << col1);
	if ((b1 & ((1ul << r2->value) | (1ul << col2))) != 0)
		return;
	unsigned long b2 = b1 | (1ul << r2->value) | (1ul << col2);
	++c[id].count2;
	for (int row3 = row2 + 1; row3 < rowSize; ++row3)
	{
		__global struct row_t* r3 = m + row3;
		if ((b2 & (1ul << r3->value)) != 0)
			continue;
		for (int colIndex3 = 0; colIndex3 < r3->colCount; ++colIndex3)
		{
			int col3 = r3->columns[colIndex3];
			if ((b2 & (1ul << col3)) != 0)
				continue;
			unsigned long b3 = b2 | (1ul << r3->value) | (1ul << col3);
			++c[id].count3;
			for (int row4 = row3 + 1; row4 < rowSize; ++row4)
			{
				__global struct row_t* r4 = m + row4;
				if ((b3 & (1ul << r4->value)) != 0)
					continue;
				for (int colIndex4 = 0; colIndex4 < r4->colCount; ++colIndex4)
				{
					int col4 = r4->columns[colIndex4];
					if ((b3 & (1ul << col4)) != 0)
						continue;
					unsigned long b4 = b3 | (1ul << r4->value) | (1ul << col4);
					++c[id].count4;
					for (int row5 = row4 + 1; row5 < rowSize; ++row5)
					{
						__global struct row_t* r5 = m + row5;
						if ((b4 & (1ul << r5->value)) != 0)
							continue;
						for (int colIndex5 = 0; colIndex5 < r5->colCount; ++colIndex5)
						{
							int col5 = r5->columns[colIndex5];
							if ((b4 & (1ul << col5)) != 0)
								continue;
							++c[id].count5;
						}
					}
				}
			}
		}
	}
}
	)";

	auto device = compute::system::default_device();
	cout << "Using device: " << device.name() << '\n';
	compute::context deviceContext{ device };
	compute::command_queue commandQueue{ deviceContext, device };

	compute::vector<row_t> mDevice{ m.size(), deviceContext };
	compute::copy(m.begin(), m.end(), mDevice.begin(), commandQueue);

	compute::vector<counts_t> countsDevice{ counts.size(), deviceContext };
	compute::copy(counts.begin(), counts.end(), countsDevice.begin(), commandQueue);

	auto program = compute::program::build_with_source(source, deviceContext);
	auto kernel = program.create_kernel("test");
	kernel.set_args(mDevice.get_buffer(), countsDevice.get_buffer(), rowSize, colSize);

	Timer executeTimer;
	commandQueue.enqueue_1d_range_kernel(kernel, 0, countSize, 0);
	commandQueue.finish();
	cout << "Execute duration: " << executeTimer.GetDurationMs() << "ms\n";

	compute::copy(countsDevice.begin(), countsDevice.end(), counts.begin(), commandQueue);

	Timer sumTimer;

	uint128_t count2 = 0, count3 = 0, count4 = 0, count5 = 0;
	for (auto& count : counts)
	{
		count2 += count.count2;
		count3 += count.count3;
		count4 += count.count4;
		count5 += count.count5;
	}

	cout << "Size: " << twoCards.size() << '\n';
	cout << "Count2: " << count2 << '\n';
	cout << "Count3: " << count3 << '\n';
	cout << "Count4: " << count4 << '\n';
	cout << "Count5: " << count5 << '\n';
	cout << "Duration: " << timer.GetDurationMs() << "ms\n";
	cout << "Sum Duration: " << sumTimer.GetDurationMs() << "ms\n";
	cout << '\n';
}
