#pragma once

class LargeOdds
{
public:
	LargeOdds() = default;
	LargeOdds(const LargeInteger& winOrDraw, const LargeInteger& lose);
	LargeOdds(const LargeOdds& rhs) = default;
	LargeOdds(LargeOdds&& rhs) = default;
	~LargeOdds() = default;

	LargeOdds& operator=(const LargeOdds& rhs) = default;
	LargeOdds& operator=(LargeOdds&& rhs) = default;

	const LargeInteger& GetWinOrDraw() const;
	const LargeInteger& GetLose() const;
	LargeInteger GetTotal() const;

	static LargeOdds Create(const LargeInteger& winOrDraw, int opponents);

private:
	LargeInteger winOrDraw = 0;
	LargeInteger lose = 0;
};
