#include "pch.h"
#include "Bitset.h"

Bitset::Bitset()
	: data{ 0, 0, 0, 0, 0 }
{
}

Bitset& Bitset::operator&=(const Bitset& rhs)
{
	for (auto index = 0; index < data.size(); ++index)
		data[index] &= rhs.data[index];
	return *this;
}

Bitset Bitset::operator&(const Bitset& rhs) const
{
	auto result{ *this };
	result &= rhs;
	return result;
}

uint64_t Bitset::Count() const
{
	uint64_t count = 0;
	for (auto part : data)
		count += ::__popcnt64(part);
	return count;
}

unsigned long Bitset::CountLeadingZeros(unsigned long start) const
{
	unsigned long offset = (start / 64) * 64;
	for (auto index = start / 64; index < data.size(); ++index, offset += 64)
		if (data[index] != 0)
			return offset + BitScan(data[index]);
	return offset;
}

Bitset& Bitset::Set(uint64_t index, bool value)
{
	auto partIndex = index / 64;
	auto position = index % 64;
	if (value)
		data[partIndex] |= (1ull << position);
	else
		data[partIndex] &= ~(1ull << position);
	return *this;
}

bool Bitset::operator[](uint64_t index) const
{
	auto partIndex = index / 64;
	auto position = index % 64;
	return (data[partIndex] & (1ull << position)) != 0;
}

unsigned long Bitset::BitScan(uint64_t value)
{
	unsigned long position = 0;
	::_BitScanForward64(&position, value);
	return position;
}
