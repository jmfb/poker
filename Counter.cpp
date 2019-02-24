#include "pch.h"
#include "Counter.h"

Counter::Counter(uint128_t value)
{
	low = value.convert_to<uint64_t>();
	value >>= 64;
	high = value.convert_to<uint64_t>();
}

Counter& Counter::operator++()
{
	if (::_addcarry_u64(0, low, 1, &low))
		++high;
	return *this;
}

Counter& Counter::operator+=(const Counter& rhs)
{
	auto carry = ::_addcarry_u64(0, low, rhs.low, &low);
	::_addcarry_u64(carry, high, rhs.high, &high);
	return *this;
}

Counter& Counter::operator+=(uint64_t value)
{
	if (::_addcarry_u64(0, low, value, &low))
		++high;
	return *this;
}

uint128_t Counter::Get() const
{
	uint128_t result{ high };
	result <<= 64;
	result |= low;
	return result;
}
