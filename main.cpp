#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <exception>
using namespace std;

#include "Math.h"
#include "Deck.h"
#include "HoleCards.h"
#include "AllHands.h"

LargeInteger ComputeTotalCombinations(LargeInteger cards, LargeInteger opponentCards)
{
	if (opponentCards < 0)
		return 0;
	if (opponentCards == 0)
		return 1;
	return Combinations(cards, opponentCards) * Partitions(opponentCards);
}

bool IsDisjoint(const vector<HoleCards>& holes, const HoleCards& hole)
{
	return all_of(holes.begin(), holes.end(), [&](const HoleCards& other) { return other.IsDisjoint(hole); });
}

LargeInteger ComputeTwoCardOverlap(
	const vector<HoleCards>& holes,
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
		if (!IsDisjoint(holes, *iter))
			continue;
		overlap += overlapPerPair;
		vector<HoleCards> newHoles{ holes };
		newHoles.push_back(*iter);
		overlap -= ComputeTwoCardOverlap(newHoles, begin, iter, remaining - 2, opponentCards - 2);
	}
	return overlap;
}

void Compute(const AllHands& allHands, const HoleCards& hole, int c1, int c2, int c3, int c4, int c5, long long& win, long long& draw, long long& lose)
{
	vector<int> cards;
	for (auto card = 0; card < DeckSize; ++card)
		cards.push_back(card);

	auto end = remove(cards.begin(), cards.end(), hole.GetCard1());
	end = remove(cards.begin(), end, hole.GetCard2());
	end = remove(cards.begin(), end, c1);
	end = remove(cards.begin(), end, c2);
	end = remove(cards.begin(), end, c3);
	end = remove(cards.begin(), end, c4);
	end = remove(cards.begin(), end, c5);

	auto bestHand = allHands.GetBestHandRank(hole.GetCard1(), hole.GetCard2(), c1, c2, c3, c4, c5);

	//All 1-card hands that beat you
	set<int> oneCards;
	for (auto c = cards.begin(); c != end; )
	{
		auto nextBestHand = allHands.GetBestHandRank(*c, c1, c2, c3, c4, c5);
		if (nextBestHand > bestHand)
		{
			oneCards.insert(*c);
			rotate(c, c + 1, end);
			--end;
		}
		else
			++c;
	}
	cout << "1-card losses: " << oneCards.size() << '\n';

	//All 2-card hands that beat you (grouped by first card)
	vector<HoleCards> twoCards;
	for (auto o1 = cards.begin(); o1 != end; ++o1)
		for (auto o2 = o1 + 1; o2 != end; ++o2)
			if (allHands.GetBestHandRank(*o1, *o2, c1, c2, c3, c4, c5) > bestHand)
				twoCards.emplace_back(*o1, *o2);
	cout << "2-card losses: " << twoCards.size() << '\n';
	cout << '\n';

	//64-bit overflow occurs before 8 opponents.  TODO: Need large integer support
	for (LargeInteger opponents = 1; opponents <= 8; ++opponents)
	{
		cout << "Opponents: " << opponents.ToString() << '\n';
		LargeInteger overlap = 0;
		for (auto iter = twoCards.begin(); iter != twoCards.end(); ++iter)
			for (auto prev = twoCards.begin(); prev != iter; ++prev)
				if (iter->IsDisjoint(*prev))
					++overlap;
		cout << "Overlap: " << overlap.ToString() << '\n';

		auto opponentCards = opponents * 2;
		cout << "Opponent Cards: " << opponentCards.ToString() << '\n';

		LargeInteger remaining = DeckSize - 2 - 5;
		auto total = ComputeTotalCombinations(remaining, opponentCards);
		cout << "Total: " << total.ToString() << '\n';

		remaining -= static_cast<long long>(oneCards.size());
		cout << "Remaining: " << remaining.ToString() << '\n';
		auto excludingOneCardLosses = ComputeTotalCombinations(remaining, opponentCards);
		cout << "Excluding 1-card losses: " << excludingOneCardLosses.ToString() << '\n';

		auto twoCardCount = ComputeTotalCombinations(remaining - 2, opponentCards - 2);
		auto twoCardLosses = twoCardCount * static_cast<long long>(twoCards.size());
		cout << "2-Card count: " << twoCards.size() << '\n';
		cout << "2-Card count per pair: " << twoCardCount.ToString() << '\n';
		cout << "2-Card losses: " << twoCardLosses.ToString() << '\n';

		LargeInteger twoCardOverlap = 0;
		for (auto iter = twoCards.begin(); iter != twoCards.end(); ++iter)
			twoCardOverlap += ComputeTwoCardOverlap({ *iter }, twoCards.begin(), iter, remaining - 2, opponentCards - 2);
		cout << "2-Card overlap: " << twoCardOverlap.ToString() << '\n';
		auto twoCardTotal = twoCardLosses - twoCardOverlap;
		cout << "2-Card total: " << twoCardTotal.ToString() << '\n';

		auto winOrDraw = excludingOneCardLosses - twoCardTotal;
		cout << "Win or draw: " << winOrDraw.ToString() << '\n';

		auto winOrDrawPercent = (winOrDraw * 1000 + 5) / total;
		cout << "Percent * 10 = " << winOrDrawPercent.ToString() << '\n';
		auto oneIn = total / winOrDraw;
		cout << "Win or draw 1 in " << oneIn.ToString() << '\n';

		cout << '\n';
	}
}

int main()
{
	try
	{
		LargeInteger::RunTests();
		return 0;

		cout << "Computing best hands...";
		AllHands allHands;
		cout << "done.\n";

		vector<int> cards;
		for (auto card = 0; card < DeckSize; ++card)
			cards.push_back(card);
		
		HoleCards hole{ Card{ Face::Ace, Suit::Spades }, Card{ Face::Jack, Suit::Spades } };
		auto end = remove(cards.begin(), cards.end(), hole.GetCard1());
		end = remove(cards.begin(), end, hole.GetCard2());

		long long win = 0;
		long long draw = 0;
		long long lose = 0;

		Card c1{ Face::Two, Suit::Hearts };
		Card c2{ Face::Three, Suit::Hearts };
		Card c3{ Face::Four, Suit::Hearts };
		Card c4{ Face::Eight, Suit::Hearts };
		Card c5{ Face::Nine, Suit::Clubs };
		Compute(allHands, hole, c1.GetId(), c2.GetId(), c3.GetId(), c4.GetId(), c5.GetId(), win, draw, lose);

		//for (auto c1 = cards.begin(); c1 != end; ++c1)
		//	for (auto c2 = c1 + 1; c2 != end; ++c2)
		//		for (auto c3 = c2 + 1; c3 != end; ++c3)
		//			for (auto c4 = c3 + 1; c4 != end; ++c4)
		//				for (auto c5 = c4 + 1; c5 != end; ++c5)
		//				{
		//					Compute(allHands, hole, *c1, *c2, *c3, *c4, *c5, win, draw, lose); return 0;
		//				}

		//cout << hole.ToString() << ": Win " << win << ", Draw " << draw << ", Lose " << lose << '\n';
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
