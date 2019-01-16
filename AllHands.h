#pragma once
#include "HandType.h"
#include <vector>
#include <map>
using namespace std;

class AllHands
{
public:
	AllHands();
	AllHands(const AllHands& rhs) = default;
	AllHands(AllHands&& rhs) = default;
	~AllHands() = default;

	AllHands& operator=(const AllHands& rhs) = default;
	AllHands& operator=(AllHands&& rhs) = default;

	size_t GetDistinctHandCount() const;
	int GetRankByHash(int hash) const;
	int GetCountByHandType(HandType type) const;
	int GetBestHandRank(int c1, int c2, int m1, int m2, int m3, int m4, int m5) const;
	int GetBestHandRank(int c, int m1, int m2, int m3, int m4, int m5) const;
	void DumpHandDistribution() const;

private:
	int GetRank(int c1, int c2, int c3, int c4, int c5) const;

private:
	vector<int> handHashByIndex;
	vector<int> handRanksByHash;
	map<HandType, int> countByHandType;
};
