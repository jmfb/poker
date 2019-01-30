#include "pch.h"
#include "LargeOddsComputer.h"
#include "Math.h"
#include "Hand.h"

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
	for (auto iter = twoCards.begin(); iter != twoCards.end(); ++iter)
		winOrDraw += ComputeTwoCardOverlap({ *iter }, twoCards.begin(), iter, cards.GetSize() - 2, opponentCards - 2);
	auto largeOdds = LargeOdds::Create(winOrDraw, opponents);
	cache.emplace(hash, largeOdds);
	AddSuitCombinations(hole, c1, c2, c3, c4, c5, largeOdds);
	return largeOdds;
}

LargeInteger LargeOddsComputer::ComputeTwoCardOverlap(
	const CardSet& holes,
	vector<HoleCards>::const_iterator begin,
	vector<HoleCards>::const_iterator end,
	LargeInteger remaining,
	LargeInteger opponentCards)
{
	if (opponentCards <= 0)
		return 0;
	auto overlapPerPair = ComputeTotalCombinations(remaining - 2, opponentCards - 2);
	LargeInteger overlap = 0;
	for (auto iter = begin; iter != end; ++iter)
	{
		if (holes.Intersect(*iter))
			continue;
		overlap += overlapPerPair;
		overlap -= ComputeTwoCardOverlap(holes.Union(*iter), begin, iter, remaining - 2, opponentCards - 2);
	}
	return overlap;
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
