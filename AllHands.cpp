#include "AllHands.h"
#include "Hand.h"
#include "Deck.h"
#include <array>
#include <iostream>
#include <algorithm>
using namespace std;

AllHands::AllHands()
{
	handRanksByHash.resize(HandHashCount);

	for (auto c1 = 0; c1 < DeckSize; ++c1)
		for (auto c2 = c1 + 1; c2 < DeckSize; ++c2)
			for (auto c3 = c2 + 1; c3 < DeckSize; ++c3)
				for (auto c4 = c3 + 1; c4 < DeckSize; ++c4)
					for (auto c5 = c4 + 1; c5 < DeckSize; ++c5)
					{
						auto hash = Hand::GetHash(c1, c2, c3, c4, c5);
						handHashByIndex.push_back(hash);

						Hand hand{ c1, c2, c3, c4, c5 };
						handRanksByHash[hash] = hand.GetValue().GetRank();

						++countByHandType[hand.GetValue().GetType()];
					}
}

size_t AllHands::GetDistinctHandCount() const
{
	return handHashByIndex.size();
}

int AllHands::GetRankByHash(int hash) const
{
	return handRanksByHash[hash];
}

int AllHands::GetCountByHandType(HandType type) const
{
	return countByHandType.find(type)->second;
}

int AllHands::GetBestHandRank(int c1, int c2, int m1, int m2, int m3, int m4, int m5) const
{
	auto rank = GetRank(m1, m2, m3, m4, m5);
	rank = max(rank, GetRank(c1, m1, m2, m3, m4));
	rank = max(rank, GetRank(c1, m1, m2, m3, m5));
	rank = max(rank, GetRank(c1, m1, m2, m4, m5));
	rank = max(rank, GetRank(c1, m1, m3, m4, m5));
	rank = max(rank, GetRank(c1, m2, m3, m4, m5));
	rank = max(rank, GetRank(c2, m1, m2, m3, m4));
	rank = max(rank, GetRank(c2, m1, m2, m3, m5));
	rank = max(rank, GetRank(c2, m1, m2, m4, m5));
	rank = max(rank, GetRank(c2, m1, m3, m4, m5));
	rank = max(rank, GetRank(c2, m2, m3, m4, m5));
	rank = max(rank, GetRank(c1, c2, m1, m2, m3));
	rank = max(rank, GetRank(c1, c2, m1, m2, m4));
	rank = max(rank, GetRank(c1, c2, m1, m2, m5));
	rank = max(rank, GetRank(c1, c2, m1, m3, m4));
	rank = max(rank, GetRank(c1, c2, m1, m3, m5));
	rank = max(rank, GetRank(c1, c2, m1, m4, m5));
	rank = max(rank, GetRank(c1, c2, m2, m3, m4));
	rank = max(rank, GetRank(c1, c2, m2, m3, m5));
	rank = max(rank, GetRank(c1, c2, m2, m4, m5));
	rank = max(rank, GetRank(c1, c2, m3, m4, m5));
	return rank;
}

void AllHands::DumpHandDistribution() const
{
	cout << GetDistinctHandCount() << " possible hands.\n";
	cout << GetCountByHandType(HandType::RoyalFlush) << " royal flushes\n";
	cout << GetCountByHandType(HandType::StraightFlush) << " straight flushes\n";
	cout << GetCountByHandType(HandType::FourOfAKind) << " four of a kinds\n";
	cout << GetCountByHandType(HandType::FullHouse) << " full houses\n";
	cout << GetCountByHandType(HandType::Flush) << " flushes\n";
	cout << GetCountByHandType(HandType::Straight) << " straights\n";
	cout << GetCountByHandType(HandType::ThreeOfAKind) << " three of a kinds\n";
	cout << GetCountByHandType(HandType::TwoPair) << " two pairs\n";
	cout << GetCountByHandType(HandType::OnePair) << " pairs\n";
	cout << GetCountByHandType(HandType::HighCard) << " high cards\n";
}

int AllHands::GetRank(int c1, int c2, int c3, int c4, int c5) const
{
	array<int, HandSize> cards{ c1, c2, c3, c4, c5 };
	sort(cards.begin(), cards.end());
	auto hash = Hand::GetHash(cards[0], cards[1], cards[2], cards[3], cards[4]);
	return GetRankByHash(hash);
}
