#include "pch.h"
#include "OpenClInt128.h"

OpenClInt128::OpenClInt128(LargeInteger value)
{
	lowlow = value.convert_to<uint32_t>();
	value >>= 32;
	lowhigh = value.convert_to<uint32_t>();
	value >>= 32;
	highlow = value.convert_to<uint32_t>();
	value >>= 32;
	highhigh = value.convert_to<uint32_t>();
}

OpenClInt128& OpenClInt128::operator+=(const OpenClInt128& rhs)
{
	auto carry = _addcarry_u32(0, lowlow, rhs.lowlow, &lowlow);
	carry = _addcarry_u32(carry, lowhigh, rhs.lowhigh, &lowhigh);
	carry = _addcarry_u32(carry, highlow, rhs.highlow, &highlow);
	_addcarry_u32(carry, highhigh, rhs.highhigh, &highhigh);
	return *this;
}

OpenClInt128 OpenClInt128::operator+(const OpenClInt128& rhs) const
{
	auto result{ *this };
	result += rhs;
	return result;
}

LargeInteger OpenClInt128::ToLargeInteger() const
{
	LargeInteger result{ highhigh };
	result <<= 32;
	result |= highlow;
	result <<= 32;
	result |= lowhigh;
	result <<= 32;
	result |= lowlow;
	return result;
}

string OpenClInt128::GetSource()
{
	return R"(
struct uint128_t
{
	unsigned int lowlow, lowhigh, highlow, highhigh;
};

void increment(__global struct uint128_t* data)
{
	asm("add.cc.u32 %0, %0, 1;" : "+r" (data->lowlow));
	asm("addc.cc.u32 %0, %0, 0;" : "+r" (data->lowhigh));
	asm("addc.cc.u32 %0, %0, 0;" : "+r" (data->highlow));
	asm("addc.u32 %0, %0, 0;" : "+r" (data->highhigh));
}
	)";
}
