#pragma once

class OpenClInt128
{
public:
	OpenClInt128() = default;
	OpenClInt128(LargeInteger value);
	OpenClInt128(const OpenClInt128& rhs) = default;
	OpenClInt128(OpenClInt128&& rhs) = default;
	~OpenClInt128() = default;

	OpenClInt128& operator=(const OpenClInt128& rhs) = default;
	OpenClInt128& operator=(OpenClInt128&& rhs) = default;

	OpenClInt128& operator+=(const OpenClInt128& rhs);
	OpenClInt128 operator+(const OpenClInt128& rhs) const;

	LargeInteger ToLargeInteger() const;
	static string GetSource();

private:
	uint32_t lowlow = 0;
	uint32_t lowhigh = 0;
	uint32_t highlow = 0;
	uint32_t highhigh = 0;
};
