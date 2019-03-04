#pragma once
#include "HandType.h"

class AllHands
{
public:
	AllHands();
	AllHands(const AllHands& rhs) = default;
	AllHands(AllHands&& rhs) = default;
	~AllHands() = default;

	AllHands& operator=(const AllHands& rhs) = default;
	AllHands& operator=(AllHands&& rhs) = default;

	int GetBestHandRank(int c1, int c2, int m1, int m2, int m3, int m4, int m5) const;
	bool BeatenByOneCard(int bestHand, int c, int m1, int m2, int m3, int m4, int m5) const;
	bool BeatenByTwoCards(int bestHand, int c1, int c2, int m1, int m2, int m3, int m4, int m5) const;

	void DumpHistogram() const;

private:
	void Compute();
	void Load();
	void Save();
	int GetRank(int c1, int c2, int c3, int c4, int c5) const;
	int GetRankByHash(int hash) const;

private:
	vector<int> handRanksByHash;
};
