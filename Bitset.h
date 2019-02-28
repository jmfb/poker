#pragma once

class Bitset
{
public:
	Bitset();
	Bitset(const Bitset& rhs) = default;
	Bitset(Bitset&& rhs) = default;
	~Bitset() = default;

	Bitset& operator=(const Bitset& rhs) = default;
	Bitset& operator=(Bitset&& rhs) = default;

	Bitset& operator&=(const Bitset& rhs);
	Bitset operator&(const Bitset& rhs) const;
	uint64_t Count() const;
	unsigned long CountLeadingZeros(unsigned long start = 0) const;
	Bitset& Set(uint64_t index, bool value = true);
	bool operator[](uint64_t index) const;

private:
	static unsigned long BitScan(uint64_t value);

private:
	array<uint64_t, 5> data; //5 * 64 = 320 bits (268 needed)
};
