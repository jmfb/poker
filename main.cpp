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

	for (auto c : oneCards)
		cout << c << ' ';
	cout << '\n';

	//All 2-card hands that beat you (grouped by first card)
	map<int, set<int>> twoCardSets;
	set<pair<int, int>> twoCards;
	vector<HoleCards> twoCardHoles;
	for (auto o1 = cards.begin(); o1 != end; ++o1)
		for (auto o2 = o1 + 1; o2 != end; ++o2)
		{
			auto nextBestHand = allHands.GetBestHandRank(*o1, *o2, c1, c2, c3, c4, c5);
			if (nextBestHand > bestHand)
			{
				twoCards.emplace(*o1, *o2);
				twoCardSets[*o1].insert(*o2);
				twoCardHoles.emplace_back(Card{ *o1 }, Card{ *o2 });
			}
		}

	auto headsUpCount = 0;
	auto matchOne = 0;
	auto matchTwo = 0;

	auto loseCount = 0;
	auto winOrDrawCount = 0;

	for (HoleCards hole2; hole2.IsValid(); hole2.MoveNext())
	{
		if (!hole2.IsDisjoint(hole) || hole2.Contains(c1) || hole2.Contains(c2) || hole2.Contains(c3) || hole2.Contains(c4) || hole2.Contains(c5))
			continue;
		++headsUpCount;
		if (oneCards.find(hole2.GetCard1()) != oneCards.end() || oneCards.find(hole2.GetCard2()) != oneCards.end())
			++matchOne;
		else
		{
			auto iter = twoCardSets.find(hole2.GetCard1());
			if (iter != twoCardSets.end() && iter->second.find(hole2.GetCard2()) != iter->second.end())
				++matchTwo;
		}

		auto nextBestHand = allHands.GetBestHandRank(hole2.GetCard1(), hole2.GetCard2(), c1, c2, c3, c4, c5);
		if (nextBestHand > bestHand)
			++loseCount;
		else
			++winOrDrawCount;
	}
	cout << headsUpCount << " Total opponents.  " << matchOne << " Beat with 1 cards, " << matchTwo << " Beat with 2 cards.\n";
	cout << "Head ups brute force: Lose " << loseCount << ", Win or Draw " << winOrDrawCount << '\n';

	//TODO: compute some stuff!

	//One-opponent computations
	auto count = DeckSize - 2 - 5;
	auto total = Combinations(count, 2);
	auto matchOneComputed = oneCards.size() * (2 * count - oneCards.size() - 1) / 2;
	auto matchTwoComputed = twoCards.size();
	auto winOrDraw = total - matchOneComputed - matchTwoComputed;
	cout << "Computed: Total " << total << ", Match One " << matchOneComputed << ", Match Two " << matchTwoComputed << ", Win Or Draw " << winOrDraw << '\n';

	vector<int> remaining;
	for (auto card = 0; card < DeckSize; ++card)
		if (card != hole.GetCard1() && card != hole.GetCard2() && card != c1 && card != c2 && card != c3 && card != c4 && card != c5)
			remaining.push_back(card);
	auto countTwoOpponent = 0;
	auto countTwoLose = 0;
	auto countTwoWinOrDraw = 0;


	auto shit = [&](const HoleCards& hole2, const HoleCards& hole3)
	{
		++countTwoOpponent;
		auto bestHand2 = allHands.GetBestHandRank(hole2.GetCard1(), hole2.GetCard2(), c1, c2, c3, c4, c5);
		auto bestHand3 = allHands.GetBestHandRank(hole3.GetCard1(), hole3.GetCard2(), c1, c2, c3, c4, c5);
		if (bestHand2 > bestHand || bestHand3 > bestHand)
			++countTwoLose;
		else
			++countTwoWinOrDraw;
	};

	auto fuck = [&](int h1, int h2, int h3, int h4)
	{
		shit(HoleCards{ Card{ h1 }, Card{ h2 } }, HoleCards{ Card{ h3 }, Card{ h4 } });
		shit(HoleCards{ Card{ h1 }, Card{ h3 } }, HoleCards{ Card{ h2 }, Card{ h4 } });
		shit(HoleCards{ Card{ h1 }, Card{ h4 } }, HoleCards{ Card{ h2 }, Card{ h3 } });
	};

	for (auto i1 = 0; i1 < remaining.size(); ++i1)
		for (auto i2 = i1 + 1; i2 < remaining.size(); ++i2)
			for (auto i3 = i2 + 1; i3 < remaining.size(); ++i3)
				for (auto i4 = i3 + 1; i4 < remaining.size(); ++i4)
					fuck(remaining[i1], remaining[i2], remaining[i3], remaining[i4]);

	cout << "Two Opponent: Count " << countTwoOpponent << ", Lose " << countTwoLose << ", Win or Draw " << countTwoWinOrDraw << '\n';

	auto computedTwoOpponent = Combinations(count, 4) * Partitions(4);
	auto computedTwoNotOne = Combinations(count - oneCards.size(), 4) * Partitions(4);
	cout << "Computed Two Count: " << computedTwoOpponent << '\n';
	cout << "Computed Two Not Losing to One: " << computedTwoNotOne << '\n';

	map<int, map<int, int>> twoCounts;
	auto shit2 = [&](int h1, int h2) -> bool
	{
		auto iter = twoCardSets.find(h1);
		if (iter == twoCardSets.end())
			return false;
		if (iter->second.find(h2) == iter->second.end())
			return false;
		++twoCounts[h1][h2];
		return true;
	};

	map<int, map<int, vector<string>>> otherTwoCounts;
	auto shit3 = [&](int h1, int h2, int h3, int h4) -> bool
	{
		auto iter = twoCardSets.find(h3);
		if (iter == twoCardSets.end())
			return true;
		if (iter->second.find(h4) == iter->second.end())
			return true;
		otherTwoCounts[h3][h4].push_back(HoleCards{ Card{ h1 }, Card{ h2 } }.ToString());

		return true;
	};

	auto remainingCount = 0;
	auto fuck2 = [&](int h1, int h2, int h3, int h4)
	{
		remainingCount += 3;
		shit2(h1, h2) && shit3(h1, h2, h3, h4) || shit2(h3, h4);
		shit2(h1, h3) && shit3(h1, h3, h2, h4) || shit2(h2, h4);
		shit2(h1, h4) && shit3(h1, h4, h2, h3) || shit2(h2, h3);
	};

	auto end2 = remaining.end();
	for (auto one : oneCards)
		end2 = remove(remaining.begin(), end2, one);
	for (auto i1 = remaining.begin(); i1 != end2; ++i1)
		for (auto i2 = i1 + 1; i2 != end2; ++i2)
			for (auto i3 = i2 + 1; i3 != end2; ++i3)
				for (auto i4 = i3 + 1; i4 != end2; ++i4)
					fuck2(*i1, *i2, *i3, *i4);

	auto rollup = 0;
	cout << "Remaining cards: " << distance(remaining.begin(), end2) << '\n';
	for (auto p : twoCounts)
	{
		auto miniRollup = 0;
		auto index1 = distance(remaining.begin(), find(remaining.begin(), end2, p.first));
		cout << Card{ p.first }.ToString() << "{" << index1 << "}: ";
		for (auto s : p.second)
		{
			auto index2 = distance(remaining.begin(), find(remaining.begin(), end2, s.first));
			auto other = otherTwoCounts[p.first][s.first];
			cout << Card{ s.first }.ToString() << "{" << index2 << "}(" << s.second << "/" << other.size() << ") ";
			//cout << "<<";
			//for (auto f : other)
			//	cout << f << ',';
			//cout << ">> ";
			rollup += s.second;
			miniRollup += s.second;
		}
		cout << "  TOTAL(" << miniRollup << ")\n";
	}
	cout << "Rollup = " << rollup << " (Remaining count " << remainingCount << ")" << '\n';

	auto x = 0;
	auto totalY = 0;
	for (auto iter = twoCardHoles.begin(); iter != twoCardHoles.end(); ++iter)
	{
		auto y = 0;
		for (auto prev = twoCardHoles.begin(); prev != iter; ++prev)
		{
			if (iter->IsDisjoint(*prev))
				++y;
		}
		totalY += y;
	}
	cout << "Y = " << totalY << '\n';

	auto computedTwoLoseTwo = Combinations(count - oneCards.size() - 2, 2) * twoCards.size() - totalY;
	cout << "Computed Two lose Two " << computedTwoLoseTwo << '\n';
	auto computedTwoWinOrDraw = computedTwoNotOne - computedTwoLoseTwo;
	cout << "Computed Two Win or Draw " << computedTwoWinOrDraw << '\n';

	//TODO: Still need to make this generic for more players.
}

int main()
{
	try
	{
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
