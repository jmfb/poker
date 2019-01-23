#include "pch.h"
#include "Math.h"
#include "Deck.h"
#include "HoleCards.h"
#include "AllHands.h"
#include "LargeOdds.h"

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
		if (!all_of(holes.begin(), holes.end(), [&](const HoleCards& other) { return other.IsDisjoint(*iter); }))
			continue;
		overlap += overlapPerPair;
		vector<HoleCards> newHoles{ holes };
		newHoles.push_back(*iter);
		overlap -= ComputeTwoCardOverlap(newHoles, begin, iter, remaining - 2, opponentCards - 2);
	}
	return overlap;
}

LargeOdds Compute(
	const AllHands& allHands,
	const HoleCards& hole,
	int c1,
	int c2,
	int c3,
	int c4,
	int c5,
	int opponents)
{
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
	return LargeOdds::Create(winOrDraw, opponents);
}

int main()
{
	try
	{
		cout << "Computing best hands...";
		AllHands allHands;
		cout << "done.\n";

		Deck deck;
		deck.NewDeck();
		
		HoleCards hole{ Card{ Face::Ace, Suit::Spades }, Card{ Face::Jack, Suit::Spades } };
		deck.Remove(hole);

		Card c1{ Face::Two, Suit::Hearts };
		Card c2{ Face::Three, Suit::Hearts };
		Card c3{ Face::Four, Suit::Hearts };
		Card c4{ Face::Eight, Suit::Hearts };
		Card c5{ Face::Nine, Suit::Clubs };
		auto result = Compute(allHands, hole, c1.GetId(), c2.GetId(), c3.GetId(), c4.GetId(), c5.GetId(), 8);

		cout << "Win or draw: " << result.GetWinOrDraw() << '\n';
		cout << "Lose: " << result.GetLose() << '\n';
		cout << "Total: " << result.GetTotal() << '\n';

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
