#include "pch.h"
#include "Math.h"
#include "Deck.h"
#include "HoleCards.h"
#include "AllHands.h"
#include "LargeOdds.h"
#include "CardSet.h"

LargeInteger ComputeTwoCardOverlap(
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

using duration = std::chrono::microseconds;
duration dNewDeck, dBestHand, dSingleCard, dTwoCard, dWinOrDraw, dOverlap;

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
	auto t1 = std::chrono::system_clock::now();
	Deck cards;
	cards.NewDeck();
	cards.Remove(hole, c1, c2, c3, c4, c5);
	auto t2 = std::chrono::system_clock::now();
	auto bestHand = allHands.GetBestHandRank(hole.GetCard1(), hole.GetCard2(), c1, c2, c3, c4, c5);
	auto t3 = std::chrono::system_clock::now();
	cards.RemoveSingleCardLosses(allHands, bestHand, c1, c2, c3, c4, c5);
	auto t4 = std::chrono::system_clock::now();
	auto twoCards = cards.FindTwoCardLosses(allHands, bestHand, c1, c2, c3, c4, c5);
	auto t5 = std::chrono::system_clock::now();
	auto opponentCards = opponents * 2;
	auto winOrDraw = ComputeTotalCombinations(cards.GetSize(), opponentCards) -
		ComputeTotalCombinations(cards.GetSize() - 2, opponentCards - 2) * static_cast<long long>(twoCards.size());
	auto t6 = std::chrono::system_clock::now();
	//TODO: remove when done optimizing
	//cout << twoCards.size() << " 2-card losses to compute overlap\n";
	for (auto iter = twoCards.begin(); iter != twoCards.end(); ++iter)
	{
		//TODO: remove when done optimizing
		//cout << distance(iter, twoCards.end()) << ": " << iter->ToString() << "...";
		winOrDraw += ComputeTwoCardOverlap({ *iter }, twoCards.begin(), iter, cards.GetSize() - 2, opponentCards - 2);
		//TODO: remove when done optimizing
		//cout << '\n';
	}
	auto t7 = std::chrono::system_clock::now();
	dNewDeck += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
	dBestHand += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);
	dSingleCard += std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3);
	dTwoCard += std::chrono::duration_cast<std::chrono::microseconds>(t5 - t4);
	dWinOrDraw += std::chrono::duration_cast<std::chrono::microseconds>(t6 - t5);
	dOverlap += std::chrono::duration_cast<std::chrono::microseconds>(t7 - t6);
	return LargeOdds::Create(winOrDraw, opponents);
}

int main()
{
	try
	{
		AllHands allHands;

		Deck deck;
		deck.NewDeck();
		
		HoleCards hole{ Card{ Face::Ace, Suit::Spades }, Card{ Face::Jack, Suit::Spades } };
		deck.Remove(hole);

		//55 2-card losses takes an exhorbitant amount of time
		//auto result = Compute(allHands, hole, 0, 1, 2, 13, 26, 1);
		//cout << "Win or draw " << result.GetWinOrDraw() << ", Lose " << result.GetLose() << '\n';

		//Card c1{ Face::Two, Suit::Hearts };
		//Card c2{ Face::Three, Suit::Hearts };
		//Card c3{ Face::Four, Suit::Hearts };
		//Card c4{ Face::Eight, Suit::Hearts };
		//Card c5{ Face::Nine, Suit::Clubs };
		//auto result = Compute(allHands, hole, c1.GetId(), c2.GetId(), c3.GetId(), c4.GetId(), c5.GetId(), 8);

		//cout << "Win or draw: " << result.GetWinOrDraw() << '\n';
		//cout << "Lose: " << result.GetLose() << '\n';
		//cout << "Total: " << result.GetTotal() << '\n';

		LargeOdds odds;
		auto opponents = 1;
		cout << "Computing odds for " << opponents << " opponents.\n";
		LargeInteger count = 0;
		auto end = deck.end();
		auto communityCount = Combinations(deck.GetSize(), 5);
		auto onePercent = communityCount / 100;
		LargeInteger percent = 0;
		for (auto c1 = deck.begin(); c1 != end; ++c1)
			for (auto c2 = c1 + 1; c2 != end; ++c2)
				for (auto c3 = c2 + 1; c3 != end; ++c3)
					for (auto c4 = c3 + 1; c4 != end; ++c4)
						for (auto c5 = c4 + 1; c5 != end; ++c5)
						{
							++count;
							auto currentPercent = count / onePercent;
							if (currentPercent > percent)
							{
								percent = currentPercent;
								cout << '.';
								if (percent % 10 == 0)
									cout << percent;
							}

							odds += Compute(allHands, hole, *c1, *c2, *c3, *c4, *c5, opponents);
						}
		cout << '\n';
		cout << hole.ToString() << ": Win or draw " << odds.GetWinOrDraw() << ", Lose " << odds.GetLose() << '\n';
		//1-opponent: Js As: Win or draw 1392530857, Lose 705041543
		cout << "Profiling metrics (time by function in Compute):\n";
		cout << "NewDeck:    " << dNewDeck.count() << "us\n";
		cout << "BestHand:   " << dBestHand.count() << "us\n";
		cout << "SingleCard: " << dSingleCard.count() << "us\n";
		cout << "TwoCard:    " << dTwoCard.count() << "us\n";
		cout << "WinOrDraw:  " << dWinOrDraw.count() << "us\n";
		cout << "Overlap:    " << dOverlap.count() << "us\n";

		/*
		Profiling metrics (time by function in Compute):
		NewDeck:    20628us (<1s)
		BestHand:   52830us (<1s)
		SingleCard: 9701579us (9s)
		>           7409371 (7s) -> 2s faster
		TwoCard:    594730269us (9.9m)
		>           331732454 (5.5m) -> 4.4m faster
		>           307024324 (5.1m) -> 0.4m faster
		WinOrDraw:  4672us (<1s)
		Overlap:    15560us (<1s)
		*/
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
