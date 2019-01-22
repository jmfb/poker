#include "LargeInteger.h"
#include <sstream>
#include <algorithm>
using namespace std;

LargeInteger::LargeInteger(long long value)
{
	if (value == 0)
		return;

	if (value < 0)
	{
		isNegative = true;
		value = -value;
	}

	for (; value != 0; value /= 10)
		data.push_back(value % 10);
}

LargeInteger LargeInteger::Absolute() const
{
	LargeInteger result;
	result.data = data;
	return result;
}

bool LargeInteger::IsNegative() const
{
	return isNegative;
}

bool LargeInteger::IsZero() const
{
	return data.empty();
}

LargeInteger& LargeInteger::operator+=(const LargeInteger& rhs)
{
	if (rhs.IsNegative())
		return operator-=(rhs.Absolute());
	if (IsNegative())
		return operator=(-(Absolute() - rhs));

	unsigned char carry = 0;
	data.resize(max(data.size(), rhs.data.size()));
	for (auto index = 0; index < data.size(); ++index)
	{
		auto result = data[index] + rhs.data[index] + carry;
		data[index] = result % 10;
		carry = result / 10;
	}
	if (carry != 0)
		data.push_back(carry);
	return *this;
}

LargeInteger& LargeInteger::operator-=(const LargeInteger& rhs)
{
	if (rhs.IsNegative())
		return operator+=(rhs.Absolute());
	if (IsNegative())
		return operator=(-(Absolute() + rhs));
	if (operator<(rhs))
		return operator=(-(rhs - *this));

	auto draw = 0;
	size_t index = 0;
	for (; index < rhs.data.size(); ++index)
	{
		auto result = data[index] - rhs.data[index] - draw;
		draw = 0;
		if (result < 0)
		{
			result += 10;
			draw = 1;
		}
		data[index] = result;
	}
	for (; draw != 0; ++index)
	{
		auto result = data[index] - draw;
		draw = 0;
		if (result < 0)
		{
			result += 10;
			draw = 1;
		}
		data[index] = result;
	}
	while (!data.empty() && data.back() == 0)
		data.pop_back();
	return *this;
}

LargeInteger& LargeInteger::operator*=(const LargeInteger& rhs)
{
	if (IsZero() || rhs.IsZero())
		return operator=({});
	auto sign = isNegative ^ rhs.isNegative;
	LargeInteger result;
	for (auto index = 0; index < rhs.data.size(); ++index)
	{
		LargeInteger addend;
		fill_n(back_inserter(addend.data), index, 0);
		addend.data.insert(addend.data.end(), data.begin(), data.end());
		unsigned char carry = 0;
		for (auto addendIndex = index; addendIndex < addend.data.size(); ++addendIndex)
		{
			auto product = addend.data[addendIndex] * rhs.data[index] + carry;
			addend.data[addendIndex] = product % 10;
			carry = product / 10;
		}
		if (carry != 0)
			addend.data.push_back(carry);
		result += addend;
	}
	result.isNegative = sign;
	return operator=(result);
}

LargeInteger& LargeInteger::operator/=(const LargeInteger& rhs)
{
	if (rhs.IsZero())
		throw runtime_error{ "Divide by zero." };
	if (IsZero() || rhs.Absolute() > Absolute())
		return operator=({});
	auto sign = isNegative ^ rhs.isNegative;
	LargeInteger result;
	//TODO: implement long division algorithm
	throw runtime_error{ "Large integer division not yet implemented." };
	result.isNegative = sign;
	return operator=(result);
}

LargeInteger LargeInteger::operator+(const LargeInteger& rhs) const
{
	auto result = *this;
	result += rhs;
	return result;
}

LargeInteger LargeInteger::operator-(const LargeInteger& rhs) const
{
	auto result = *this;
	result -= rhs;
	return result;
}

LargeInteger LargeInteger::operator*(const LargeInteger& rhs) const
{
	auto result = *this;
	result *= rhs;
	return result;
}

LargeInteger LargeInteger::operator/(const LargeInteger& rhs) const
{
	auto result = *this;
	result /= rhs;
	return result;
}

LargeInteger LargeInteger::operator-() const
{
	auto result = *this;
	result.isNegative = !result.isNegative;
	if (result.data.empty())
		result.isNegative = false;
	return result;
}

LargeInteger& LargeInteger::operator++()
{
	return operator+=(1);
}

LargeInteger LargeInteger::operator++(int)
{
	auto result = *this;
	operator+=(1);
	return result;
}

LargeInteger& LargeInteger::operator--()
{
	return operator-=(1);
}

LargeInteger LargeInteger::operator--(int)
{
	auto result = *this;
	operator-=(1);
	return result;
}

bool LargeInteger::operator==(const LargeInteger& rhs) const
{
	return isNegative == rhs.isNegative && data == rhs.data;
}

bool LargeInteger::operator!=(const LargeInteger& rhs) const
{
	return !operator==(rhs);
}

bool LargeInteger::operator<(const LargeInteger& rhs) const
{
	if (IsNegative())
	{
		if (rhs.IsNegative())
			return Absolute() > rhs.Absolute();
		return true;
	}
	if (rhs.IsNegative())
		return false;
	if (data.size() < rhs.data.size())
		return true;
	if (data.size() > rhs.data.size())
		return false;
	for (vector<unsigned char>::const_reverse_iterator liter = data.rbegin(), riter = rhs.data.rbegin(); liter != data.rend() && riter != rhs.data.rend(); ++liter, ++riter)
		if (*liter < *riter)
			return true;
		else if (*liter > *riter)
			return false;
	return false;
}

bool LargeInteger::operator<=(const LargeInteger& rhs) const
{
	return !(rhs < *this);
}

bool LargeInteger::operator>(const LargeInteger& rhs) const
{
	return rhs < *this;
}

bool LargeInteger::operator>=(const LargeInteger& rhs) const
{
	return !operator<(rhs);
}

string LargeInteger::ToString() const
{
	ostringstream out;
	if (isNegative)
		out << '-';
	for (auto iter = data.rbegin(); iter != data.rend(); ++iter)
		out << static_cast<char>(static_cast<char>(*iter) + '0');
	if (data.empty())
		out << '0';
	return out.str();
}

void LargeInteger::RunTests()
{
	LargeInteger defaultZero;
	LargeInteger zero{ 0 };
	if (defaultZero != zero || zero.ToString() != "0" || !zero.IsZero() || zero.IsNegative())
		throw runtime_error{ "Default should be zero." };
	auto negativeZero = -zero;
	if (negativeZero.IsNegative() || negativeZero != zero || !negativeZero.IsZero())
		throw runtime_error{ "Negative zero is still not negative." };

	LargeInteger one{ 1 };
	if (one == zero || one < zero || one <= zero || zero > one || zero >= one || one.IsNegative() || one.IsZero())
		throw runtime_error{ "One should be greater than zero." };
	if (one.ToString() != "1")
		throw runtime_error{ "One should be 1: " + one.ToString() };

	auto negativeOne = -one;
	if (negativeOne == zero || negativeOne == one || !negativeOne.IsNegative() || negativeOne.IsZero() || negativeOne.ToString() != "-1")
		throw runtime_error{ "Negative one should not be one or zero." };
	if (negativeOne >= zero || negativeOne > zero || negativeOne >= one || negativeOne > one)
		throw runtime_error{ "Negative one should not be greater than zero or one." };
	if (negativeOne.Absolute() != one)
		throw runtime_error{ "Absolute value of negative one should be one." };

	LargeInteger two{ 2 };
	auto onePlusOne = one + one;
	if (two.ToString() != "2" || two != onePlusOne || two <= one)
		throw runtime_error{ "Two should be equal to one plus one." };

	if ((LargeInteger{ 99 } + one).ToString() != "100")
		throw runtime_error{ "Carry did not cascade." };

	if ((LargeInteger{ 10 } - LargeInteger{ 12 }).ToString() != "-2")
		throw runtime_error{ "Subtracting to get a negative number." };
	if ((LargeInteger{ 1 } - LargeInteger{ -1 }).ToString() != "2")
		throw runtime_error{ "Subtracting a negative number is addition." };

	if ((LargeInteger{ 100000 } - LargeInteger{ 99999 }).ToString() != "1")
		throw runtime_error{ "Subtracting number to borrow and eliminate leading zeroes failed." };

	if ((LargeInteger{ 1234 } * LargeInteger{ -6789 }) != LargeInteger{ 1234 * -6789 })
		throw runtime_error{ "Multiplication has some isues." };
	if ((LargeInteger{ -10 } * LargeInteger{ -43 }) != LargeInteger{ -10 * -43 })
		throw runtime_error{ "Multiplying two negative numbers." };

	if (!(LargeInteger{ 1 } / LargeInteger{ 2 }).IsZero())
		throw runtime_error{ "Integer division to zero failed." };
	if ((LargeInteger{ 65432 } / LargeInteger{ -321 }) != LargeInteger{ 65432 / -321 })
		throw runtime_error{ "Integer division failed." };
}
