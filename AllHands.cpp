#include "pch.h"
#include "AllHands.h"
#include "Hand.h"
#include "Deck.h"

const auto fileName = "handRanksByHash.bin";

AllHands::AllHands()
{
	if (!std::experimental::filesystem::is_regular_file(fileName))
	{
		cout << "Computing best hands...";
		Compute();
		cout << "done.\n";
		cout << "Saving to disk...";
		Save();
		cout << "done.\n";
	}
	else
	{
		cout << "Loading best hands from disk...";
		Load();
		cout << "done.\n";
	}
}

void AllHands::Compute()
{
	handRanksByHash.resize(HandHashCount);
	for (auto c1 = 0; c1 < DeckSize; ++c1)
		for (auto c2 = c1 + 1; c2 < DeckSize; ++c2)
			for (auto c3 = c2 + 1; c3 < DeckSize; ++c3)
				for (auto c4 = c3 + 1; c4 < DeckSize; ++c4)
					for (auto c5 = c4 + 1; c5 < DeckSize; ++c5)
						handRanksByHash[Hand::GetHash(c1, c2, c3, c4, c5)] = Hand{ c1, c2, c3, c4, c5 }.GetValue().GetRank();
}

void AllHands::Load()
{
	ifstream in{ fileName, ios::binary };
	handRanksByHash.resize(HandHashCount);
	in.read(reinterpret_cast<char*>(handRanksByHash.data()), HandHashCount * sizeof(int));
}

void AllHands::Save()
{
	ofstream out{ fileName, ios::binary };
	out.write(reinterpret_cast<const char*>(handRanksByHash.data()), HandHashCount * sizeof(int));
}

int AllHands::GetRankByHash(int hash) const
{
	return handRanksByHash[hash];
}

int AllHands::GetBestHandRank(const GamePlayer& player, const Community& community) const
{
	return GetBestHandRank(player.GetHole(), community);
}

int AllHands::GetBestHandRank(const HoleCards& hole, const Community& community) const
{
	return GetBestHandRank(
		hole.GetCard1(),
		hole.GetCard2(),
		community[0],
		community[1],
		community[2],
		community[3],
		community[4]);
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

bool AllHands::BeatenByOneCard(int bestHand, int c, int m1, int m2, int m3, int m4, int m5) const
{
	return GetRank(c, m1, m2, m3, m4) > bestHand ||
		GetRank(c, m1, m2, m3, m5) > bestHand ||
		GetRank(c, m1, m2, m4, m5) > bestHand ||
		GetRank(c, m1, m3, m4, m5) > bestHand ||
		GetRank(c, m2, m3, m4, m5) > bestHand;
}

bool AllHands::BeatenByTwoCards(int bestHand, int c1, int c2, int m1, int m2, int m3, int m4, int m5) const
{
	return GetRank(c1, c2, m1, m2, m3) > bestHand ||
		GetRank(c1, c2, m1, m2, m4) > bestHand ||
		GetRank(c1, c2, m1, m2, m5) > bestHand ||
		GetRank(c1, c2, m1, m3, m4) > bestHand ||
		GetRank(c1, c2, m1, m3, m5) > bestHand ||
		GetRank(c1, c2, m1, m4, m5) > bestHand ||
		GetRank(c1, c2, m2, m3, m4) > bestHand ||
		GetRank(c1, c2, m2, m3, m5) > bestHand ||
		GetRank(c1, c2, m2, m4, m5) > bestHand ||
		GetRank(c1, c2, m3, m4, m5) > bestHand;
}

void AllHands::DumpHistogram() const
{
	map<HandType, int> histogram;
	for (auto c1 = 0; c1 < DeckSize; ++c1)
		for (auto c2 = c1 + 1; c2 < DeckSize; ++c2)
			for (auto c3 = c2 + 1; c3 < DeckSize; ++c3)
				for (auto c4 = c3 + 1; c4 < DeckSize; ++c4)
					for (auto c5 = c4 + 1; c5 < DeckSize; ++c5)
						++histogram[HandValue{ handRanksByHash[Hand::GetHash(c1, c2, c3, c4, c5)] }.GetType()];
	for (auto p : histogram)
		cout << ToString(p.first) << ": " << p.second << '\n';
}

int AllHands::GetRank(int c1, int c2, int c3, int c4, int c5) const
{
	array<int, HandSize> cards{ c1, c2, c3, c4, c5 };
	sort(cards.begin(), cards.end());
	auto hash = Hand::GetHash(cards[0], cards[1], cards[2], cards[3], cards[4]);
	return GetRankByHash(hash);
}
