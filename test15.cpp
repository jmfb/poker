#include "pch.h"
#include "HoleCards.h"
#include "Counts.h"
#include "Timer.h"

//Duration: 1473.24ms
//Count1: 196
//Count2: 16569
//Count3: 1253794
//Count4: 82408790
//Count5: 5021768178

class Bitset
{
public:
	Bitset()
		: data{ 0, 0, 0, 0, 0 }
	{
	}
	Bitset(const Bitset& rhs) = default;
	Bitset(Bitset&& rhs) = default;
	~Bitset() = default;

	Bitset& operator=(const Bitset& rhs) = default;
	Bitset& operator=(Bitset&& rhs) = default;

	Bitset& operator&=(const Bitset& rhs)
	{
		for (auto index = 0; index < data.size(); ++index)
			data[index] &= rhs.data[index];
		return *this;
	}
	Bitset operator&(const Bitset& rhs) const
	{
		auto result{ *this };
		result &= rhs;
		return result;
	}

	uint64_t Count() const
	{
		uint64_t count = 0;
		for (auto part : data)
			count += ::__popcnt64(part);
		return count;
	}

	unsigned long CountLeadingZeros(unsigned long start = 0) const
	{
		unsigned long offset = 0;
		for (auto index = start / 64; index < data.size(); ++index, offset += 64)
			if (data[index] != 0)
				return offset + BitScan(data[index]);
		return offset;
	}

	Bitset& Set(uint64_t index, bool value = true)
	{
		auto partIndex = index / 64;
		auto position = index % 64;
		if (value)
			data[partIndex] |= (1ull << position);
		else
			data[partIndex] &= ~(1ull << position);
		return *this;
	}

	bool operator[](uint64_t index) const
	{
		auto partIndex = index / 64;
		auto position = index % 64;
		return (data[partIndex] & (1ull << position)) != 0;
	}

private:
	static unsigned long BitScan(uint64_t value)
	{
		unsigned long position = 0;
		::_BitScanForward64(&position, value);
		return position;
	}

private:
	array<uint64_t, 5> data; //5 * 64 = 320 bits (268 needed)
};

vector<pair<int, int>> CreateTwoCards();

void Test15()
{
	cout << "Test15: (std)bitset, (boost)dynamic_bitset, and custom (intrinsic)bitset comparisons.\n";

	Timer timer;

	vector<uint64_t> data;
	for (auto& twoCard : CreateTwoCards())
		data.push_back((1ull << twoCard.first) | (1ull << twoCard.second));

	Counts counts;
	vector<Bitset> m(data.size());
	for (auto x = 0; x < (data.size() - 1); ++x)
		for (auto y = x + 1; y < data.size(); ++y)
			if ((data[x] & data[y]) == 0)
				m[x].Set(y);

	Bitset r1, r2, r3, r4;
	unsigned long i2 = 0, i3 = 0, i4 = 0;
	uint64_t c2 = 0, c3 = 0, c4 = 0;
	for (auto i1 = 0; i1 < (m.size() - 1); ++i1)
	{
		r1 = m[i1];
		c2 = r1.Count();
		if (c2 == 0)
			continue;
		counts.count2 += static_cast<uint32_t>(c2);
		for (i2 = i1; c2--; )
		{
			i2 = r1.CountLeadingZeros(i2 + 1);
			r1.Set(i2, false);
			r2 = r1 & m[i2];
			c3 = r2.Count();
			if (c3 == 0)
				continue;
			counts.count3 += static_cast<uint32_t>(c3);
			for (i3 = i2; c3--; )
			{
				i3 = r2.CountLeadingZeros(i3 + 1);
				r2.Set(i3, false);
				r3 = r2 & m[i3];
				c4 = r3.Count();
				if (c4 == 0)
					continue;
				counts.count4 += static_cast<uint32_t>(c4);
				for (i4 = i3; c4--; )
				{
					i4 = r3.CountLeadingZeros(i4 + 1);
					r3.Set(i4, false);
					counts.count5 += (r3 & m[i4]).Count();
				}
			}
		}
	}

	cout << "Duration: " << timer.GetDurationMs() << "ms\n";
	cout << "Count1: " << data.size() << '\n';
	counts.Dump();
}
