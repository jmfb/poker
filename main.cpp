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

		map<int, map<int, int>> sevens;

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

		auto sevenSpades = Card{ Face::King, Suit::Spades }.GetId();
		auto sevenClubs = Card{ Face::Ace, Suit::Clubs }.GetId();

		auto test = [&](int h1, int h2, int h3, int h4, int h5, int h6)
		{
			if (h1 == h2 || h1 == h3 || h1 == h4 || h1 == h5 || h1 == h6 ||
				h2 == h3 || h2 == h4 || h2 == h5 || h2 == h6 ||
				h3 == h4 || h3 == h5 || h3 == h6 ||
				h4 == h5 || h4 == h6 ||
				h5 == h6)
			{
				cout << h1 << h2 << ' ' << h3 << h4 << ' ' << h5 << h6 << " WTF!::!:!L!LL!\n";
				throw runtime_error{ "Stop fucking shit up in your stupid manual partitions." };
			}

			++c;
			//7s 7c
			if (lose(h1, h2))
			{
				++l;
				if (h3 == sevenSpades && h4 == sevenClubs)
					++sevens[h1][h2];
				else if (h5 == sevenSpades && h6 == sevenClubs)
					++sevens[h1][h2];
			}
			else if (lose(h3, h4))
			{
				++l;
				if (h5 == sevenSpades && h6 == sevenClubs)
					++sevens[h3][h4];
			}
			else if (lose(h5, h6))
			{
				++l;
			}
			else
				++wod;


			//if (lose(h1, h2) || lose(h3, h4) || lose(h5, h6))
			//	++l;
			//else
			//	++wod;
		};

		auto fuck3 = [&](int h1, int h2, int h3, int h4, int h5, int h6)
		{
			if (h1 == h2 || h1 == h3 || h1 == h4 || h1 == h5 || h1 == h6 ||
				h2 == h3 || h2 == h4 || h2 == h5 || h2 == h6 ||
				h3 == h4 || h3 == h5 || h3 == h6 ||
				h4 == h5 || h4 == h6 ||
				h5 == h6)
			{
				cout << h1 << h2 << ' ' << h3 << h4 << ' ' << h5 << h6 << " WTF!::!:!L!LL!\n";
				throw runtime_error{ "Stop fucking shit up in your stupid fucking loops." };
			}

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

		7s 7c - This is the first one that deviates from the 153 expectation.
		5s: 6s(153) 5c(153) 6c(153) Ac(153) 5d(153) 6d(153) Ad(153) -0
		6s: 5c(148) 6c(148) 5d(148) 6d(148) -5

		Qs Qc - This is the last one that would match a deviation based on previous disjoint count
		5s: 6s(153) 5c(153) 6c(153) Ac(153) 5d(153) 6d(153) Ad(153) -0
		6s: 5c(148) 6c(148) 5d(148) 6d(148) -5
		7s: 7c(142) 7d(142) -11
		Ts: Tc(140) Td(140) -13

		Qs Ac - This is the first one that differs from previous disjoin count
		5s: 6s(153) 5c(153) 6c(153) 5d(153) 6d(153) Ad(153) -0
		6s: 5c(149) 6c(149) 5d(149) 6d(149) -4 (instead of -5, so +1 for some reason)
		7s: 7c(143) 7d(143) -10 (instead of -11, so +1 for some reason)
		Ts: Tc(141) Td(141) -12 (instead of -13, so +1 for some reason)

		Ks Ac - First one off by 11 (previous only off by 8)
		5s: 6s(153) 5c(153) 6c(153) 5d(153) 6d(153) Ad(153) -0
		6s: 5c(149) 6c(149) 5d(149) 6d(149) -4 (instead of -5 so +1)
		7s: 7c(143) 7d(143) -10 (instead of -11 so +1)
		Ts: Tc(141) Td(141) -12 (instead of -13 so +1)
		Qs: Qc(139) Qd(139) Ad(140) -14, -14, -13 (instead of -15/-15/-14, so +1)

		5c 6c - First one just completely off the rails
		5s: 6s(153) Ac(153) 5d(153) 6d(153) Ad(153) -0
		6s: 5d(150) 6d(150) -3 (instead of -5, so +2 for some reason)
		7s: 7c(146) 7d(146) -7 (instead of -11, so +4 for some reason)
		Ts: Tc(144) Td(144) -9 (instead of -13, so +4 for some reason)
		Qs: Qc(142) Ac(143) Qd(142) Ad(143) -11, -10, -11, -10 (instead of -15/-14/-15/-14 so +4 for some reason)
		Ks: Kc(138) Ac(140) Kd(138) Ad(140) -15, -13, -15, -13 (instead of -19/-17/-19/-17 so +4 for some reason)
		*/

		cout << "Sevens summary:\n";
		for (auto& seven : sevens)
		{
			cout << Card{ seven.first }.ToString() << ": ";
			for (auto& x : seven.second)
				cout << Card{ x.first }.ToString() << "(" << x.second << ") ";
			cout << '\n';
		}
	}
}

int main()
{
	try
	{
		if (true)
		{
			map<char, map<char, int>> losses;

			set<pair<char, char>> twoCards
			{
				{ '1', '2' }, { '2', '6' }, { '2', '8' }, { '2', 'T' }, { '6', '7' }, { '6', '8' }, { '6', '9' }, { '6', 'T' }, { '8', 'T' }
			};

			auto isTwoCard = [&](char c1, char c2) -> bool
			{
				return twoCards.find({ c1, c2 }) != twoCards.end();
			};

			auto lose = [&](char c1, char c2) -> bool
			{
				if (!isTwoCard(c1, c2))
					return false;
				++losses[c1][c2];
				return true;
			};

			auto c = 0;
			auto l = 0;
			auto wod = 0;

			auto test = [&](char c1, char c2, char c3, char c4, char c5, char c6)
			{
				++c;
				if (lose(c1, c2) || lose(c3, c4) || lose(c5, c6))
					++l;
				else
					++wod;
			};

			auto fuck = [&](char c1, char c2, char c3, char c4, char c5, char c6)
			{
				test(c1, c2, c3, c4, c5, c6);
				test(c1, c2, c3, c5, c4, c6);
				test(c1, c2, c3, c6, c4, c5);

				test(c1, c3, c2, c4, c5, c6);
				test(c1, c3, c2, c5, c4, c6);
				test(c1, c3, c2, c6, c4, c5);

				test(c1, c4, c2, c3, c5, c6);
				test(c1, c4, c2, c5, c3, c6);
				test(c1, c4, c2, c6, c3, c5);

				test(c1, c5, c2, c3, c4, c6);
				test(c1, c5, c2, c4, c3, c6);
				test(c1, c5, c2, c6, c3, c4);

				test(c1, c6, c2, c3, c4, c5);
				test(c1, c6, c2, c4, c3, c5);
				test(c1, c6, c2, c5, c3, c4);
			};

			vector<char> cards{ '1', '2', '6', '7', '8', '9', 'T' };
			for (auto i1 = 0; i1 < cards.size(); ++i1)
				for (auto i2 = i1 + 1; i2 < cards.size(); ++i2)
					for (auto i3 = i2 + 1; i3 < cards.size(); ++i3)
						for (auto i4 = i3 + 1; i4 < cards.size(); ++i4)
							for (auto i5 = i4 + 1; i5 < cards.size(); ++i5)
								for (auto i6 = i5 + 1; i6 < cards.size(); ++i6)
									fuck(cards[i1], cards[i2], cards[i3], cards[i4], cards[i5], cards[i6]);


			/*
			Remaining Cards minus 8T = { 12679 }
			12 67 (12 loss) NOTE: Also 67 loss \___This is the +2 !!!!
			12 69 (12 loss) NOTE: Also 69 loss /
			12 79 (12 loss)
			16 27 *
			16 29 *
			16 79 *
			17 26 (26 loss)
			17 29 *
			17 69 (69 loss)
			19 26 (26 loss)
			19 27 *
			19 67 (67 loss)
			26 79 (26 loss)
			27 69 (69 loss)
			29 67 (67 loss)

			8T prev disjoint { 12, 26, 67, 69 } Disjoint 2-partitions { 12 67, 12 69 } = 2

			//TODO: Implement algorithm
			*/

			cout << "Count " << c << '\n';
			cout << "Loss " << l << '\n';
			cout << "Win or draw " << wod << '\n';
			for (auto x : losses)
			{
				cout << x.first << ": ";
				for (auto y : x.second)
					cout << y.first << "(" << y.second << ") ";
				cout << '\n';
			}

			//TODO: Generate l (95) from twoCards {A2 26 28 2T 67 68 69 6T 8T} (9)
			//Using previous algorithm: 5C4P4*9 = 15*9 = 135, subtract PD*3C2 = 14*3 = 42 ... 135-42 = 93, add PD2P = 2 ... 93+2 = 95 (check)
			//Is there a more elegant forward generative solution?
			//OC = O*2 = 3*2 = 6, D = { A26789T } (7)
			//Two Card Map: { { A: 2 }, { 2: 6 8 T }, { 6: 7 8 9 }, { 8: T } }
			//Total OPC = 7C6P6 = 7*15 = 105
			//If A (1/7 * 105 = 15) Ax xx xx and (6/7 * 105 = 90) {26789T} xx xx xx
			//. A2 {6789T} xx xx is loss (?/15) <- already can't figure this one out.  Previous algorithm should work....fingers crossed.

			return 0;
		}

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
