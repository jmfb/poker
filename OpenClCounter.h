#pragma once

class OpenClCounter
{
public:
	OpenClCounter() = default;
	OpenClCounter(uint128_t value);
	OpenClCounter(const OpenClCounter& rhs) = default;
	OpenClCounter(OpenClCounter&& rhs) = default;
	~OpenClCounter() = default;

	OpenClCounter& operator=(const OpenClCounter& rhs) = default;
	OpenClCounter& operator=(OpenClCounter&& rhs) = default;

	OpenClCounter& operator+=(const OpenClCounter& rhs);
	OpenClCounter operator+(const OpenClCounter& rhs) const;

	uint128_t Get() const;
	static string GetSource();

private:
	uint32_t lowlow = 0;
	uint32_t lowhigh = 0;
	uint32_t highlow = 0;
	uint32_t highhigh = 0;
};
