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

long long ComputeTotalCombinations(long long cards, long long opponentCards)
{
	if (opponentCards < 0)
		return 0;
	if (opponentCards == 0)
		return 1;
	return Combinations(cards, opponentCards) * Partitions(opponentCards);
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

	for (auto opponents = 1; opponents <= 3; ++opponents)
	{
		cout << "Opponents: " << opponents << '\n';
		auto overlap = 0;
		for (auto iter = twoCards.begin(); iter != twoCards.end(); ++iter)
			for (auto prev = twoCards.begin(); prev != iter; ++prev)
				if (iter->IsDisjoint(*prev))
					++overlap;
		cout << "Overlap: " << overlap << '\n';

		auto opponentCards = opponents * 2;
		cout << "Opponent Cards: " << opponentCards << '\n';

		auto remaining = DeckSize - 2 - 5;
		auto total = ComputeTotalCombinations(DeckSize - 2 - 5, opponentCards);
		cout << "Total: " << total << '\n';

		remaining -= static_cast<decltype(remaining)>(oneCards.size());
		cout << "Remaining: " << remaining << '\n';
		auto excludingOneCardLosses = ComputeTotalCombinations(remaining, opponentCards);
		cout << "Excluding 1-card losses: " << excludingOneCardLosses << '\n';

		//auto guess = ComputeTotalCombinations(remaining - 4, opponentCards - 4);
		//cout << "Guess: " << guess << '\n';
		//auto weight = opponents == 1 ? 0 : opponents == 2 ? 1 : guess;
		//cout << "Weight: " << weight << '\n';
		auto x = ComputeTotalCombinations(remaining - 2, opponentCards - 2);
		cout << "X: " << x << '\n';
		//auto subtract = overlap * weight;
		auto subtract = opponents == 1 ? 0 : opponents == 2 ? overlap : opponents == 3 ? 8074 : 0;
		auto twoCardLosses = x * twoCards.size() - subtract;
		cout << "2-card losses: " << twoCardLosses << '\n';
		auto winOrDraw = excludingOneCardLosses - twoCardLosses;
		cout << "Win or draw: " << winOrDraw << '\n';
		cout << '\n';
	}

	map<int, set<int>> twoCardSets;
	for (auto p : twoCards)
		twoCardSets[p.GetCard1()].insert(p.GetCard2());

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
	for (auto iter = twoCards.begin(); iter != twoCards.end(); ++iter)
	{
		auto y = 0;
		for (auto prev = twoCards.begin(); prev != iter; ++prev)
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

	cout << "Brute forcing 3-opponents (this may take a while)...\n";

	{
		auto c = 0;
		auto wod = 0;
		auto l = 0;

		map<int, map<int, int>> twoLosses;

		auto lose = [&](int h1, int h2) -> bool
		{
			auto iter = twoCardSets.find(h1);
			if (iter == twoCardSets.end())
				return false;
			if (iter->second.find(h2) == iter->second.end())
				return false;
			++twoLosses[h1][h2];
			return true;
		};

		auto test = [&](int h1, int h2, int h3, int h4, int h5, int h6)
		{
			++c;
			if (lose(h1, h2) || lose(h3, h4) || lose(h5, h6))
				++l;
			else
				++wod;
		};

		auto fuck3 = [&](int h1, int h2, int h3, int h4, int h5, int h6)
		{
			test(h1, h2, h3, h4, h5, h6);
			test(h1, h2, h3, h5, h4, h6);
			test(h1, h2, h3, h6, h4, h5);
			test(h1, h3, h2, h4, h5, h6);
			test(h1, h3, h2, h5, h4, h6);
			test(h1, h3, h2, h6, h4, h5);
			test(h1, h4, h2, h3, h5, h6);
			test(h1, h4, h2, h5, h3, h6);
			test(h1, h4, h2, h6, h3, h5);
			test(h1, h5, h2, h3, h4, h6);
			test(h1, h5, h2, h4, h3, h6);
			test(h1, h5, h2, h6, h3, h4);
			test(h1, h6, h2, h3, h4, h5);
			test(h1, h6, h2, h4, h3, h5);
			test(h1, h6, h2, h5, h3, h4);
		};

		for (auto i1 = remaining.begin(); i1 != end2; ++i1)
			for (auto i2 = i1 + 1; i2 != end2; ++i2)
				for (auto i3 = i2 + 1; i3 != end2; ++i3)
					for (auto i4 = i3 + 1; i4 != end2; ++i4)
						for (auto i5 = i4 + 1; i5 != end2; ++i5)
							for (auto i6 = i5 + 1; i6 != end2; ++i6)
								fuck3(*i1, *i2, *i3, *i4, *i5, *i6);

		cout << "3-Op Two Card Count: " << c << '\n';
		cout << "3-Op Lose: " << l << '\n';
		cout << "3-Op Win or Draw: " << wod << '\n';

		auto guess = 153;
		auto stuff = 14535;
		long long sum = 0;
		long long timesSum = 0;
		for (auto p : twoLosses)
		{
			cout << Card{ p.first }.ToString() << ": ";
			for (auto s : p.second)
			{
				auto less = stuff - s.second;
				auto times = less / guess;
				auto leftover = less % guess;
				timesSum += times;
				cout << Card{ s.first }.ToString() << "(" << s.second << "/" << less << "(" << times << "g+" << leftover << ")" << ") ";
				sum += s.second;
			}
			cout << '\n';
		}

		cout << "Sum of losses = " << sum << '\n';
		cout << "Times Sum: " << timesSum << '\n';
		auto upper = stuff * twoCards.size();
		cout << "Stuff: " << stuff << '\n';
		cout << "2-Cards: " << twoCards.size() << '\n';
		cout << "Upper: " << upper << '\n';
		auto diff = upper - sum;
		cout << "Difference: " << diff << '\n';
		cout << "Guess: " << guess << '\n';
		cout << "Diff mod guess: " << (diff % guess) << '\n';
		cout << "Diff / guess: " << (diff / guess) << '\n';
		auto overlap = 856;
		cout << "Overlap: " << overlap << '\n';
		auto calculated = overlap * guess;
		cout << "Calculated: " << calculated << '\n';
		if (calculated > diff)
		{
			auto overshoot = calculated - diff;
			cout << "Overshot by: " << overshoot << '\n'; //7998
			//Overshot by 7998.  This means for any given card, instead of subtracting C(18,2)=153 for previous disjoint cards I should subtract less.
			//First set of cards is exact.
			//Second set of cards is subtracted by 153 exactly.
			//Starting with third set, we've reduced subtraction by some amount.
		}
		else
		{
			auto undershoot = diff - calculated;
			cout << "Undershot by: " << undershoot << '\n';
		}

		cout << "Disjoint analysis:\n";
		auto magicSum = 0;
		map<int, map<int, int>> wtf;
		for (auto iter = twoCards.begin(); iter != twoCards.end(); ++iter)
		{
			auto wtfSum = 0;
			auto prevDisjoint = 0;
			for (auto prev = twoCards.begin(); prev != iter; ++prev)
				if (iter->IsDisjoint(*prev))
				{
					++prevDisjoint;
					wtfSum += wtf[prev->GetCard1()][prev->GetCard2()];
				}
			wtf[iter->GetCard1()][iter->GetCard2()] = prevDisjoint;
			auto actual = stuff - twoLosses[iter->GetCard1()][iter->GetCard2()];
			auto expected = prevDisjoint * guess;
			auto magic = expected - actual;
			magicSum += magic;
			cout << iter->ToString() << ": Previous disjoint " << prevDisjoint << ", Expected: " << expected << ", Actual: " << actual << ", Magic: " << magic << ", WTF " << wtfSum << '\n';
		}
		cout << "Magic Sum: " << magicSum << '\n';

		/* Close.  The WTF works for a while, then strays a little, goes back to working, then skews off
		5s 6s: Previous disjoint 0, Expected: 0, Actual: 0, Magic: 0, WTF 0
		5s 5c: Previous disjoint 0, Expected: 0, Actual: 0, Magic: 0, WTF 0
		5s 6c: Previous disjoint 0, Expected: 0, Actual: 0, Magic: 0, WTF 0
		5s Ac: Previous disjoint 0, Expected: 0, Actual: 0, Magic: 0, WTF 0
		5s 5d: Previous disjoint 0, Expected: 0, Actual: 0, Magic: 0, WTF 0
		5s 6d: Previous disjoint 0, Expected: 0, Actual: 0, Magic: 0, WTF 0
		5s Ad: Previous disjoint 0, Expected: 0, Actual: 0, Magic: 0, WTF 0
		6s 5c: Previous disjoint 5, Expected: 765, Actual: 765, Magic: 0, WTF 0
		6s 6c: Previous disjoint 5, Expected: 765, Actual: 765, Magic: 0, WTF 0
		6s 5d: Previous disjoint 5, Expected: 765, Actual: 765, Magic: 0, WTF 0
		6s 6d: Previous disjoint 5, Expected: 765, Actual: 765, Magic: 0, WTF 0
		7s 7c: Previous disjoint 11, Expected: 1683, Actual: 1663, Magic: 20, WTF 20
		7s 7d: Previous disjoint 11, Expected: 1683, Actual: 1663, Magic: 20, WTF 20
		Ts Tc: Previous disjoint 13, Expected: 1989, Actual: 1947, Magic: 42, WTF 42
		Ts Td: Previous disjoint 13, Expected: 1989, Actual: 1947, Magic: 42, WTF 42
		Qs Qc: Previous disjoint 15, Expected: 2295, Actual: 2227, Magic: 68, WTF 68
		Qs Ac: Previous disjoint 14, Expected: 2142, Actual: 2082, Magic: 60, WTF 68 <-- First deviance (+8 from Magic) (Also first non-pair after pairs with magic)
		Qs Qd: Previous disjoint 15, Expected: 2295, Actual: 2227, Magic: 68, WTF 68
		Qs Ad: Previous disjoint 14, Expected: 2142, Actual: 2082, Magic: 60, WTF 68 <-- +8 again
		Ks Kc: Previous disjoint 19, Expected: 2907, Actual: 2781, Magic: 126, WTF 126
		Ks Ac: Previous disjoint 17, Expected: 2601, Actual: 2500, Magic: 101, WTF 112 <-- +11
		Ks Kd: Previous disjoint 19, Expected: 2907, Actual: 2781, Magic: 126, WTF 126
		Ks Ad: Previous disjoint 17, Expected: 2601, Actual: 2500, Magic: 101, WTF 112 <-- +11
		5c 6c: Previous disjoint 19, Expected: 2907, Actual: 2771, Magic: 136, WTF 188 <-- +52 (off the rails)
		5c Ac: Previous disjoint 18, Expected: 2754, Actual: 2631, Magic: 123, WTF 162 <-- +39
		5c 5d: Previous disjoint 19, Expected: 2907, Actual: 2771, Magic: 136, WTF 188 <-- +52
		*/
	}
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
