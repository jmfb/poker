#include "pch.h"
#include "OpenClInt128.h"

OpenClInt128::OpenClInt128(LargeInteger value)
{
	lower = 0x7fffffffffffffffull & value.convert_to<uint64_t>();
	value >>= 63;
	upper = 0x7fffffffffffffffull & value.convert_to<uint64_t>();
	value >>= 63;
	top = value.convert_to<uint8_t>();
}

LargeInteger OpenClInt128::ToLargeInteger() const
{
	LargeInteger result{ top };
	result <<= 63;
	result |= upper;
	result <<= 63;
	result |= lower;
	return result;
}

string OpenClInt128::GetSource()
{
	return R"(
struct uint128_t
{
	unsigned char top;
	unsigned long upper;
	unsigned long lower;
};

void increment(__global struct uint128_t* data)
{
	++data->lower;
	if (data->lower == 0x8000000000000000ul)
	{
		data->lower = 0;
		++data->upper;
		if (data->upper == 0x8000000000000000ul)
		{
			data->upper = 0;
			++data->top;
		}
	}
}
	)";
}
