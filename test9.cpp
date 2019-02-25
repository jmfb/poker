#include "pch.h"
#include "Timer.h"
#include "Deck.h"
#include "Counts.h"

vector<pair<int, int>> CreateTwoCards();

const auto MaxByte = numeric_limits<uint8_t>::max();

class Column
{
public:
	Column() = default;
	Column(const Column& rhs) = default;
	Column(Column&& rhs) = default;
	~Column() = default;

	Column& operator=(const Column& rhs) = default;
	Column& operator=(Column&& rhs) = default;

	void Set(uint8_t value, uint8_t prev, uint8_t next)
	{
		this->prev = prev;
		bit = (1ull << value);
		this->next = next;
	}

	uint8_t GetNext() const
	{
		return next;
	}

	uint8_t GetPrev() const
	{
		return prev;
	}

	void SetNext(uint8_t value)
	{
		next = value;
	}

	void SetPrev(uint8_t value)
	{
		prev = value;
	}

	uint64_t GetBit() const
	{
		return bit;
	}

	void Clear()
	{
		prev = MaxByte;
		bit = 0;
		next = MaxByte;
	}

private:
	uint8_t prev = MaxByte;
	uint64_t bit = 0;
	uint8_t next = MaxByte;
};

class Row
{
public:
	Row() = default;
	Row(const Row& rhs) = default;
	Row(Row&& rhs) = default;
	~Row() = default;

	Row& operator=(const Row& rhs) = default;
	Row& operator=(Row&& rhs) = default;

	void Set(uint8_t value)
	{
		this->value = value;
		bit = (1ull << value);
	}

	uint8_t GetValue() const
	{
		return value;
	}

	uint8_t GetColumnCount() const
	{
		return size;
	}

	bool IsEmpty() const
	{
		return size == 0;
	}

	uint8_t Count(uint8_t column) const
	{
		return columns[column].GetBit() == 0 ? 0 : 1;
	}

	void AddColumn(uint8_t value)
	{
		++size;
		auto prev = MaxByte;
		auto next = first;
		while (value > next)
		{
			prev = next;
			next = columns[next].GetNext();
		}
		columns[value].Set(value, prev, next);
		if (prev == MaxByte)
			first = value;
		else
			columns[prev].SetNext(value);
		if (next != MaxByte)
			columns[next].SetPrev(value);
	}

	void RemoveColumn(uint8_t value)
	{
		auto& column = columns[value];
		if (column.GetBit() == 0)
			return;
		--size;
		auto prev = column.GetPrev();
		auto next = column.GetNext();
		column.Clear();
		if (prev == MaxByte)
			first = next;
		else
			columns[prev].SetNext(next);
		if (next != MaxByte)
			columns[next].SetPrev(prev);
	}

	uint8_t GetFirstColumn() const
	{
		return first;
	}

	uint8_t GetNextColumn(uint8_t column) const
	{
		return columns[column].GetNext();
	}

	const Column& operator[](uint8_t column) const
	{
		return columns[column];
	}

	uint64_t GetBit() const
	{
		return bit;
	}

private:
	uint8_t value = 0;
	uint64_t bit = 0;
	uint8_t first = MaxByte;
	uint8_t size = 0;
	array<Column, DeckSize> columns;
};

class Matrix
{
public:
	Matrix()
	{
		fill(rowIndexById.begin(), rowIndexById.end(), MaxByte);
	}
	Matrix(const Matrix& rhs) = default;
	Matrix(Matrix&& rhs) = default;
	~Matrix() = default;

	Matrix& operator=(const Matrix& rhs) = default;
	Matrix& operator=(Matrix&& rhs) = default;

	Row& Merge(uint8_t value)
	{
		auto index = rowIndexById[value];
		if (index != MaxByte)
			return rows[index];
		index = size++;
		rowIndexById[value] = index;
		auto& row = rows[index];
		row.Set(value);
		return row;
	}

	void Add(uint8_t row, uint8_t column)
	{
		Merge(row).AddColumn(column);
	}

	void Sort()
	{
		sort(execution::par_unseq, rows.begin(), rows.begin() + size, [](const Row& left, const Row& right)
		{
			return (left.GetColumnCount() > right.GetColumnCount()) ||
				(left.GetColumnCount() == right.GetColumnCount() && left.GetValue() < right.GetValue());
		});
		for (uint8_t index = 0; index < size; ++index)
			rowIndexById[rows[index].GetValue()] = index;
	}

	uint8_t GetRowCount() const
	{
		return size;
	}

	Counts GetCounts() const
	{
		Counts counts;
		for (uint8_t row1 = 0; row1 < size; ++row1)
		{
			auto& r1 = rows[row1];
			for (auto col1 = r1.GetFirstColumn(); col1 != MaxByte; col1 = r1.GetNextColumn(col1))
			{
				auto& c1 = r1[col1];
				auto b1 = r1.GetBit() | c1.GetBit();
				for (auto row2 = row1 + 1; row2 < size; ++row2)
				{
					auto& r2 = rows[row2];
					if ((b1 & r2.GetBit()) != 0)
						continue;
					for (auto col2 = r2.GetFirstColumn(); col2 != MaxByte; col2 = r2.GetNextColumn(col2))
					{
						auto& c2 = r2[col2];
						if ((b1 & c2.GetBit()) != 0)
							continue;
						auto b2 = b1 | r2.GetBit() | c2.GetBit();
						++counts.count2;
						for (auto row3 = row2 + 1; row3 < size; ++row3)
						{
							auto& r3 = rows[row3];
							if ((b2 & r3.GetBit()) != 0)
								continue;
							for (auto col3 = r3.GetFirstColumn(); col3 != MaxByte; col3 = r3.GetNextColumn(col3))
							{
								auto& c3 = r3[col3];
								if ((b2 & c3.GetBit()) != 0)
									continue;
								auto b3 = b2 | r3.GetBit() | c3.GetBit();
								++counts.count3;
								for (auto row4 = row3 + 1; row4 < size; ++row4)
								{
									auto& r4 = rows[row4];
									if ((b3 & r4.GetBit()) != 0)
										continue;
									for (auto col4 = r4.GetFirstColumn(); col4 != MaxByte; col4 = r4.GetNextColumn(col4))
									{
										auto& c4 = r4[col4];
										if ((b3 & c4.GetBit()) != 0)
											continue;
										auto b4 = b3 | r4.GetBit() | c4.GetBit();
										++counts.count4;
										for (auto row5 = row4 + 1; row5 < size; ++row5)
										{
											auto& r5 = rows[row5];
											if ((b4 & r5.GetBit()) != 0)
												continue;
											for (auto col5 = r5.GetFirstColumn(); col5 != MaxByte; col5 = r5.GetNextColumn(col5))
											{
												auto& c5 = r5[col5];
												if ((b4 & c5.GetBit()) != 0)
													continue;
												++counts.count5;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		return counts;
	}

	Counts GetCountsParallelRows4() const
	{
		auto size64 = static_cast<uint64_t>(size);
		auto countSize = size64 * size64 * size64 * size64;
		vector<Counts> counts(countSize);
		for_each(execution::par_unseq, make_counting_iterator(0ull), make_counting_iterator(countSize), [&](auto index)
		{
			auto& count = counts[index];
			auto remainder = index;
			auto row1 = static_cast<uint8_t>(remainder % size);
			remainder /= size;
			auto row2 = static_cast<uint8_t>(remainder % size);
			remainder /= size;
			auto row3 = static_cast<uint8_t>(remainder % size);
			remainder /= size;
			auto row4 = static_cast<uint8_t>(remainder % size);

			//Count1
			auto r1 = rows[row1];
			if (row2 == 0 && row3 == 0 && row4 == 0)
				return;
			
			//Count2
			if (row2 <= row1)
				return;
			auto r2 = rows[row2];
			r1.RemoveColumn(r2.GetValue());
			if (r1.IsEmpty())
				return;
			r2.RemoveColumn(r1.GetValue());
			if (r2.IsEmpty())
				return;
			if (row3 == 0 && row4 == 0)
			{
				for (auto col1 = r1.GetFirstColumn(); col1 != MaxByte; col1 = r1.GetNextColumn(col1))
					count.count2 += r2.GetColumnCount() - r2.Count(col1);
				return;
			}

			//Count3
			if (row3 <= row2)
				return;
			auto r3 = rows[row3];
			r1.RemoveColumn(r3.GetValue());
			if (r1.IsEmpty())
				return;
			r2.RemoveColumn(r3.GetValue());
			if (r2.IsEmpty())
				return;
			r3.RemoveColumn(r1.GetValue());
			r3.RemoveColumn(r2.GetValue());
			if (r3.IsEmpty())
				return;
			if (row4 == 0)
			{
				for (auto col1 = r1.GetFirstColumn(); col1 != MaxByte; col1 = r1.GetNextColumn(col1))
					for (auto col2 = r2.GetFirstColumn(); col2 != MaxByte; col2 = r2.GetNextColumn(col2))
						if (col2 != col1)
							count.count3 += r3.GetColumnCount() - r3.Count(col1) - r3.Count(col2);
				return;
			}

			//Count4 and Count5
			if (row4 <= row3)
				return;
			auto r4 = rows[row4];
			r1.RemoveColumn(r4.GetValue());
			if (r1.IsEmpty())
				return;
			r2.RemoveColumn(r4.GetValue());
			if (r2.IsEmpty())
				return;
			r3.RemoveColumn(r4.GetValue());
			if (r3.IsEmpty())
				return;
			r4.RemoveColumn(r1.GetValue());
			r4.RemoveColumn(r2.GetValue());
			r4.RemoveColumn(r3.GetValue());
			if (r4.IsEmpty())
				return;
			auto b0 = r1.GetBit() | r2.GetBit() | r3.GetBit() | r4.GetBit();
			for (auto col1 = r1.GetFirstColumn(); col1 != MaxByte; col1 = r1.GetNextColumn(col1))
			{
				auto& c1 = r1[col1];
				auto b1 = b0 | c1.GetBit();
				for (auto col2 = r2.GetFirstColumn(); col2 != MaxByte; col2 = r2.GetNextColumn(col2))
				{
					auto& c2 = r2[col2];
					if ((b1 & c2.GetBit()) != 0)
						continue;
					auto b2 = b1 | c2.GetBit();
					for (auto col3 = r3.GetFirstColumn(); col3 != MaxByte; col3 = r3.GetNextColumn(col3))
					{
						auto& c3 = r3[col3];
						if ((b2 & c3.GetBit()) != 0)
							continue;
						auto b3 = b2 | c3.GetBit();
						for (auto col4 = r4.GetFirstColumn(); col4 != MaxByte; col4 = r4.GetNextColumn(col4))
						{
							auto& c4 = r4[col4];
							if ((b3 & c4.GetBit()) != 0)
								continue;
							auto b4 = b3 | c4.GetBit();
							++count.count4;
							for (auto row5 = row4 + 1; row5 < size; ++row5)
							{
								auto& r5 = rows[row5];
								if ((b4 & r5.GetBit()) != 0)
									continue;
								count.count5 += r5.GetColumnCount() -
									r5.Count(r1.GetValue()) -
									r5.Count(r2.GetValue()) -
									r5.Count(r3.GetValue()) -
									r5.Count(r4.GetValue()) -
									r5.Count(col1) -
									r5.Count(col2) -
									r5.Count(col3) -
									r5.Count(col4);
							}
						}
					}
				}
			}
		});
		return Counts::GetTotal(counts);
	}

private:
	array<uint8_t, DeckSize> rowIndexById;
	uint8_t size = 0;
	array<Row, DeckSize> rows;
};

Matrix CreateTwoCardMatrix()
{
	Matrix m;
	for (auto& twoCard : CreateTwoCards())
		m.Add(static_cast<uint8_t>(twoCard.first), static_cast<uint8_t>(twoCard.second));
	m.Sort();
	return m;
}

void Test9()
{
	cout << "Test9: Wide-memory matrix solution (single threaded)\n";
	Timer createTimer;
	auto m = CreateTwoCardMatrix();
	cout << "Size of 2-card matrix: " << sizeof(m) << '\n';
	cout << "Rows: " << static_cast<int>(m.GetRowCount()) << '\n';
	cout << "Create duration: " << createTimer.GetDurationMs() << "ms\n";
	cout << '\n';

	//Counting in series (single-threaded):
	//Count1: 196
	//Count2: 16569
	//Count3: 800361
	//Count4: 24670043
	//Count5: 513145502
	//Duration: 2434.7ms (2051.4ms)
	Timer seriesTimer;
	cout << "Counting in series (single-threaded):\n";
	auto countsSeries = m.GetCounts();
	countsSeries.Dump();
	cout << "Duration: " << seriesTimer.GetDurationMs() << "ms\n";
	cout << '\n';

	//Counting in parallel (unwrapping 4 row loops):
	//Count1: 196
	//Count2: 16569
	//Count3: 800361
	//Count4: 24670043
	//Count5: 513145502
	//Duration: 856.33ms (442.536ms)
	Timer parallelTimer;
	cout << "Counting in parallel (unwrapping 4 row loops):\n";
	auto countsParallel = m.GetCountsParallelRows4();
	countsParallel.Dump();
	cout << "Duration: " << parallelTimer.GetDurationMs() << "ms\n";
	cout << '\n';
}
