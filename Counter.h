#pragma once

class Counter
{
public:
	Counter() = default;
	Counter(uint128_t value);
	Counter(const Counter& rhs) = default;
	Counter(Counter&& rhs) = default;
	~Counter() = default;

	Counter& operator=(const Counter& rhs) = default;
	Counter& operator=(Counter&& rhs) = default;

	Counter& operator++();
	Counter& operator+=(const Counter& rhs);
	Counter& operator+=(uint64_t value);

	uint128_t Get() const;

private:
	uint64_t low = 0;
	uint64_t high = 0;
};
