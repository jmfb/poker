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
