#pragma once

class LargeOdds
{
public:
	LargeOdds() = default;
	LargeOdds(int opponents, uint128_t winOrDraw, uint128_t lose);
	LargeOdds(const LargeOdds& rhs) = default;
	LargeOdds(LargeOdds&& rhs) = default;
	~LargeOdds() = default;

	LargeOdds& operator=(const LargeOdds& rhs) = default;
	LargeOdds& operator=(LargeOdds&& rhs) = default;

	int GetOpponents() const;
	uint128_t GetWinOrDraw() const;
	uint128_t GetLose() const;
	uint128_t GetTotal() const;
	double GetWinOrDrawPercent() const;

	uint128_t GetCoinFlip() const;
	double GetCoinFlipDeltaPercent() const;

	static LargeOdds Create(uint128_t winOrDraw, int opponents);

	LargeOdds& operator+=(const LargeOdds& rhs);

private:
	int opponents = 0;
	uint128_t winOrDraw = 0;
	uint128_t lose = 0;
};
