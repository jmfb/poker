#pragma once
#include <string>
#include <vector>
using namespace std;

class LargeInteger
{
public:
	LargeInteger() = default;
	LargeInteger(long long value);
	LargeInteger(const LargeInteger& rhs) = default;
	LargeInteger(LargeInteger&& rhs) = default;
	~LargeInteger() = default;

	LargeInteger& operator=(const LargeInteger& rhs) = default;
	LargeInteger& operator=(LargeInteger&& rhs) = default;

	LargeInteger Absolute() const;
	bool IsNegative() const;
	bool IsZero() const;

	LargeInteger& operator+=(const LargeInteger& rhs);
	LargeInteger& operator-=(const LargeInteger& rhs);
	LargeInteger& operator*=(const LargeInteger& rhs);
	LargeInteger& operator/=(const LargeInteger& rhs);

	LargeInteger operator+(const LargeInteger& rhs) const;
	LargeInteger operator-(const LargeInteger& rhs) const;
	LargeInteger operator*(const LargeInteger& rhs) const;
	LargeInteger operator/(const LargeInteger& rhs) const;

	LargeInteger operator-() const;

	LargeInteger& operator++();
	LargeInteger operator++(int);
	LargeInteger& operator--();
	LargeInteger operator--(int);

	bool operator==(const LargeInteger& rhs) const;
	bool operator!=(const LargeInteger& rhs) const;
	bool operator<(const LargeInteger& rhs) const;
	bool operator<=(const LargeInteger& rhs) const;
	bool operator>(const LargeInteger& rhs) const;
	bool operator>=(const LargeInteger& rhs) const;

	string ToString() const;

	static void RunTests();

private:
	bool isNegative = false;
	vector<unsigned char> data;
};
