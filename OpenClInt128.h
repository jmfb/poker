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

	OpenClInt128& operator++();
	LargeInteger ToLargeInteger() const;
	static string GetSource();

private:
	uint8_t top = 0;
	uint64_t upper = 0;
	uint64_t lower = 0;
};
