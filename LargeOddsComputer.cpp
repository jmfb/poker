#include "pch.h"
#include "LargeOddsComputer.h"
#include "Math.h"
#include "Hand.h"
#include "TwoCardOverlap.h"
#include "Timer.h"

LargeOddsComputer::LargeOddsComputer(const AllHands& allHands, const HoleCards& hole, int opponents)
{
	Deck deck;
	deck.NewDeck();
	deck.Remove(hole);
	auto end = deck.end();
	for (auto c1 = deck.begin(); c1 != end; ++c1)
		for (auto c2 = c1 + 1; c2 != end; ++c2)
			for (auto c3 = c2 + 1; c3 != end; ++c3)
				for (auto c4 = c3 + 1; c4 != end; ++c4)
					for (auto c5 = c4 + 1; c5 != end; ++c5)
						odds += ComputeCommunity(allHands, hole, *c1, *c2, *c3, *c4, *c5, opponents);
}

const LargeOdds& LargeOddsComputer::GetOdds() const
{
	return odds;
}

LargeOdds LargeOddsComputer::ComputeCommunity(
	const AllHands& allHands,
	const HoleCards& hole,
	int c1,
	int c2,
	int c3,
	int c4,
	int c5,
	int opponents)
{
	auto hash = Hand::GetHash(c1, c2, c3, c4, c5);
	auto cacheIter = cache.find(hash);
	if (cacheIter != cache.end())
		return cacheIter->second;
	Deck cards;
	cards.NewDeck();
	cards.Remove(hole, c1, c2, c3, c4, c5);
	auto bestHand = allHands.GetBestHandRank(hole.GetCard1(), hole.GetCard2(), c1, c2, c3, c4, c5);
	cards.RemoveSingleCardLosses(allHands, bestHand, c1, c2, c3, c4, c5);
	auto twoCards = cards.FindTwoCardLosses(allHands, bestHand, c1, c2, c3, c4, c5);
	auto opponentCards = opponents * 2;
	auto winOrDraw = ComputeTotalCombinations(cards.GetSize(), opponentCards) -
		ComputeTotalCombinations(cards.GetSize() - 2, opponentCards - 2) * static_cast<long long>(twoCards.size());
	auto totalOverlap = TwoCardOverlap::Compute(twoCards, cards.GetSize(), opponents);
	auto largeOdds = LargeOdds::Create(winOrDraw, opponents);
	cache.emplace(hash, largeOdds);
	AddSuitCombinations(hole, c1, c2, c3, c4, c5, largeOdds);
	return largeOdds;
}

void LargeOddsComputer::AddSuitCombinations(const HoleCards& hole, Card c1, Card c2, Card c3, Card c4, Card c5, const LargeOdds& largeOdds)
{
	set<Suit> holeSuits;
	Card h1{ hole.GetCard1() };
	Card h2{ hole.GetCard2() };
	holeSuits.insert(h1.GetSuit());
	holeSuits.insert(h2.GetSuit());

	set<Suit> communitySuits;
	communitySuits.insert(c1.GetSuit());
	communitySuits.insert(c2.GetSuit());
	communitySuits.insert(c3.GetSuit());
	communitySuits.insert(c4.GetSuit());
	communitySuits.insert(c5.GetSuit());

	set<Suit> freeSuits{ Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds };

	for (auto suit : holeSuits)
	{
		communitySuits.erase(suit);
		freeSuits.erase(suit);
	}
	for (auto suit : communitySuits)
		freeSuits.erase(suit);

	if (communitySuits.empty())
		return;

	if (holeSuits.size() == 1)
	{
		if (communitySuits.size() == 1)
		{
			auto suit1 = *communitySuits.begin();
			auto suit2 = *freeSuits.begin();
			auto suit3 = *next(freeSuits.begin());
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit1, suit2 } });
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit1, suit3 } });
		}
		else if (communitySuits.size() == 2)
		{
			auto suit1 = *communitySuits.begin();
			auto suit2 = *next(communitySuits.begin());
			auto suit3 = *freeSuits.begin();
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit1, suit3 } });
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit2, suit3 } });
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit1, suit2 }, { suit2, suit3 } });
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit1, suit3 }, { suit2, suit1 } });
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit1, suit2 }, { suit2, suit1 } });
		}
		else //3
		{
			auto suit1 = *communitySuits.begin();
			auto suit2 = *next(communitySuits.begin());
			auto suit3 = *next(communitySuits.begin(), 2);
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit1, suit2 }, { suit2, suit1 } });
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit1, suit3 }, { suit3, suit1 } });
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit2, suit3 }, { suit3, suit2 } });
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit1, suit2 }, { suit2, suit3 }, { suit3, suit1 } });
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit1, suit3 }, { suit2, suit1 }, { suit3, suit2 } });
		}
	}
	else //2
	{
		if (communitySuits.size() == 1)
		{
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { *communitySuits.begin(), *freeSuits.begin() } });
		}
		else //2
		{
			auto suit1 = *communitySuits.begin();
			auto suit2 = *next(communitySuits.begin());
			AddSuitCombination(c1, c2, c3, c4, c5, largeOdds, { { suit1, suit2 }, { suit2, suit1 } });
		}
	}
}

void LargeOddsComputer::AddSuitCombination(Card c1, Card c2, Card c3, Card c4, Card c5, const LargeOdds& largeOdds, const map<Suit, Suit>& suitMap)
{
	for (auto suits : suitMap)
	{
		c1.SwitchSuit(suits.first, suits.second);
		c2.SwitchSuit(suits.first, suits.second);
		c3.SwitchSuit(suits.first, suits.second);
		c4.SwitchSuit(suits.first, suits.second);
		c5.SwitchSuit(suits.first, suits.second);
	}
	array<int, HandSize> cards{ c1.GetId(), c2.GetId(), c3.GetId(), c4.GetId(), c5.GetId() };
	sort(cards.begin(), cards.end());
	auto hash = Hand::GetHash(cards[0], cards[1], cards[2], cards[3], cards[4]);
	cache.emplace(hash, largeOdds);
}

void LargeOddsComputer::Compute(ostream& out, const AllHands& allHands, int f1, int f2, int opponents)
{
	auto index = f1 * FaceCount + f2 + 1;
	auto count = FaceCount * FaceCount;
	Timer timer;

	auto createHole = [](int f1, int f2) -> HoleCards
	{
		if (f1 == f2)
			return { Card{ static_cast<Face>(f1), Suit::Spades }, Card{ static_cast<Face>(f2), Suit::Hearts } };
		if (f1 < f2)
			return { Card{ static_cast<Face>(f1), Suit::Spades }, Card{ static_cast<Face>(f2), Suit::Spades } };
		return { Card{ static_cast<Face>(f2), Suit::Spades }, Card{ static_cast<Face>(f1), Suit::Hearts } };
	};

	auto hole = createHole(f1, f2);
	cout << opponents << ": " << index << " of " << count << ": Computing " << hole.ToString() << "...";
	LargeOddsComputer computer{ allHands, hole, opponents };
	auto odds = computer.GetOdds();
	cout << " Win or draw " << odds.GetWinOrDraw() << ", Lose " << odds.GetLose() << ", Duration " << timer.GetDurationMs() << "ms\n";
	out << hole.GetCard1() << ','
		<< hole.GetCard2() << ','
		<< hole.ToString() << ','
		<< odds.GetWinOrDraw() << ','
		<< odds.GetLose() << ','
		<< odds.GetTotal() << endl;
}

//About 10hours to compute odds for a given opponent spread
void LargeOddsComputer::Compute(int opponents)
{
	AllHands allHands;
	ofstream out{ to_string(opponents) + "-opponent-odds.txt" };
	for (auto f1 = 0; f1 < FaceCount; ++f1)
		for (auto f2 = 0; f2 < FaceCount; ++f2)
			Compute(out, allHands, f1, f2, opponents);
}

int LargeOddsComputer::Compute(int argc, char** argv)
{
	if (argc < 2)
	{
		cout << "Enter the number of players.\n";
		return -1;
	}

	auto opponents = stoi(argv[1]);
	if (opponents < 1 || opponents > 8)
	{
		cout << "Please enter a number of opponents between 1 and 8.\n";
		return -1;
	}

	Compute(opponents);
	return 0;
}

void LargeOddsComputer::CombineOpponentOdds()
{
	map<pair<int, int>, map<int, LargeOdds>> allOdds;
	for (auto opponents = 1; opponents <= 8; ++opponents)
	{
		ifstream in{ to_string(opponents) + "-opponent-odds.txt" };
		string line;
		while (getline(in, line))
		{
			istringstream parts{ line };
			int card1 = 0, card2 = 0;
			char comma;
			string name;
			uint128_t winOrDraw = 0, lose = 0;
			parts >> card1 >> comma >> card2 >> comma;
			getline(parts, name, ',');
			parts >> winOrDraw >> comma >> lose;
			allOdds[{ card1, card2 }][opponents] = { winOrDraw, lose };
		}
	}

	ofstream out{ "all-odds.txt" };
	out << "hand,1,2,3,4,5,6,7,8\n";
	for (auto& hand : allOdds)
	{
		out << hand.first.first << ',' << hand.first.second << ',' << HoleCards{ { hand.first.first }, { hand.first.second } }.ToHandString();
		for (auto& odds : hand.second)
			out << ',' << odds.second.GetWinOrDrawPercent();
		out << '\n';
	}
}
