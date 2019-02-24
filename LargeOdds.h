#pragma once

class LargeOdds
{
public:
	LargeOdds() = default;
	LargeOdds(uint128_t winOrDraw, uint128_t lose);
	LargeOdds(const LargeOdds& rhs) = default;
	LargeOdds(LargeOdds&& rhs) = default;
	~LargeOdds() = default;

	LargeOdds& operator=(const LargeOdds& rhs) = default;
	LargeOdds& operator=(LargeOdds&& rhs) = default;

	uint128_t GetWinOrDraw() const;
	uint128_t GetLose() const;
	uint128_t GetTotal() const;
	double GetWinOrDrawPercent() const;

	static LargeOdds Create(uint128_t winOrDraw, int opponents);

	LargeOdds& operator+=(const LargeOdds& rhs);

private:
	uint128_t winOrDraw = 0;
	uint128_t lose = 0;
};
