#include "pch.h"
#include "Math.h"
#include "Deck.h"
#include "Hand.h"
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
duration dLookup, dNewDeck, dBestHand, dSingleCard, dTwoCard, dWinOrDraw, dOverlap, dSuits;

map<int, LargeOdds> computeCache;

void AddSuitCombination(Card c1, Card c2, Card c3, Card c4, Card c5, const LargeOdds& largeOdds, const map<Suit, Suit>& suitMap)
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
	computeCache.emplace(hash, largeOdds);
}

void SuitCombinations(const HoleCards& hole, Card c1, Card c2, Card c3, Card c4, Card c5, const LargeOdds& largeOdds)
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
	auto t0 = std::chrono::system_clock::now();
	auto hash = Hand::GetHash(c1, c2, c3, c4, c5);
	auto cacheIter = computeCache.find(hash);
	if (cacheIter != computeCache.end())
	{
		dLookup += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - t0);
		return cacheIter->second;
	}
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
	auto largeOdds = LargeOdds::Create(winOrDraw, opponents);
	computeCache.emplace(hash, largeOdds);
	SuitCombinations(hole, c1, c2, c3, c4, c5, largeOdds);
	auto t8 = std::chrono::system_clock::now();
	dLookup += std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
	dNewDeck += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
	dBestHand += std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);
	dSingleCard += std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3);
	dTwoCard += std::chrono::duration_cast<std::chrono::microseconds>(t5 - t4);
	dWinOrDraw += std::chrono::duration_cast<std::chrono::microseconds>(t6 - t5);
	dOverlap += std::chrono::duration_cast<std::chrono::microseconds>(t7 - t6);
	dSuits += std::chrono::duration_cast<std::chrono::microseconds>(t8 - t7);
	return largeOdds;
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
		auto opponents = 8;
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
		cout << "Lookup:     " << dLookup.count() << "us\n";
		cout << "NewDeck:    " << dNewDeck.count() << "us\n";
		cout << "BestHand:   " << dBestHand.count() << "us\n";
		cout << "SingleCard: " << dSingleCard.count() << "us\n";
		cout << "TwoCard:    " << dTwoCard.count() << "us\n";
		cout << "WinOrDraw:  " << dWinOrDraw.count() << "us\n";
		cout << "Overlap:    " << dOverlap.count() << "us\n";
		cout << "Suits:      " << dSuits.count() << "us\n";

		/*
		5-opponent
		Js As: Win or draw 1680753304654557525, Lose 4706729642656107675
		Profiling metrics (time by function in Compute):
		Lookup:     7782us
		NewDeck:    11703us
		BestHand:   33261us
		SingleCard: 4868621us
		TwoCard:    190165164us
		WinOrDraw:  12091us
		Overlap:    14246427us
		Suits:      7513303us

		4-opponent
		Js As: Win or draw 15149906882314455, Lose 32804169298696545
		Profiling metrics (time by function in Compute):
		Lookup:     8194us
		NewDeck:    13536us
		BestHand:   33766us
		SingleCard: 4942005us
		TwoCard:    190423648us
		WinOrDraw:  14581us
		Overlap:    14044587us
		Suits:      7480872us

		3-opponent (int128)
		Js As: Win or draw 99914292102666, Lose 158947117781334
		Profiling metrics (time by function in Compute):
		Lookup:     7412us
		NewDeck:    11284us
		BestHand:   36404us
		SingleCard: 4969095us
		TwoCard:    191376113us
		WinOrDraw:  11874us
		Overlap:    13790938us (WTF - 8s slower than long long)
		Suits:      7474007us

		3-opponent (long long)
		Js As: Win or draw 99914292102666, Lose 158947117781334
		Profiling metrics (time by function in Compute):
		Lookup:     7603us
		NewDeck:    10011us
		BestHand:   29555us
		SingleCard: 4910894us
		TwoCard:    191151593us
		WinOrDraw:  5710us
		Overlap:    5803328us (1.4s slower than 2-opponent)
		Suits:      7170409us

		2-opponent
		Js As: Win or draw 464067579609, Lose 482986358991
		Profiling metrics (time by function in Compute):
		Lookup:     7395us
		NewDeck:    10418us
		BestHand:   33271us
		SingleCard: 4865065us
		TwoCard:    189115210us
		WinOrDraw:  5308us
		Overlap:    4467117us (4s slower than 1-opponent)
		Suits:      7100645us
		*/

		/*
		Profiling metrics (time by function in Compute):
		Lookup:     5285us
		NewDeck:    15868us
		BestHand:   53943us
		SingleCard: 7486113us
		TwoCard:    304092458us
		WinOrDraw:  6739us
		Overlap:    11932us

		Lookup:     7144us
		NewDeck:    10137us
		BestHand:   29810us
		SingleCard: 4891224us (2.6s faster)
		TwoCard:    191823527us (1.8m faster, still takes 3.2m)
		WinOrDraw:  3677us
		Overlap:    7805us
		Suits:      7010770us (7s slower)

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
