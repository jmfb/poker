#include "pch.h"
#include "AllHands.h"
#include "HoleCards.h"
#include "Card.h"
#include "Suit.h"
#include "Face.h"
#include "LargeOddsComputer.h"
#include "HandValue.h"
#include "PreflopOdds.h"
#include "Timer.h"
#include "Math.h"
#include "TwoCardOverlap.h"

class GamePlayer
{
public:
	void SetCard1(Deck& deck)
	{
		hole.SetCard1(deck.DealCard());
	}
	void SetCard2(Deck& deck)
	{
		hole.SetCard2(deck.DealCard());
	}

private: public:
	HoleCards hole;
};

class BurnAndTurn
{
public:
	BurnAndTurn(Deck& deck, int count)
	{
		burn = deck.DealCard();
		for (auto index = 0; index < count; ++index)
			cards.push_back(deck.DealCard());
	}

private: public:
	int burn = 0;
	vector<int> cards;
};

class Game
{
public:
	void AddPlayer()
	{
		players.push_back({});
	}
	void DealPlayers()
	{
		deck.NewDeck();
		deck.Shuffle();
		for (auto& player : players)
			player.SetCard1(deck);
		for (auto& player : players)
			player.SetCard2(deck);
	}
	void DealFlop()
	{
		flop = { deck, 3 };
	}
	void DealTurn()
	{
		turn = { deck, 1 };
	}
	void DealRiver()
	{
		river = { deck, 1 };
	}

	void Dump(const AllHands& allHands, const PreflopOdds& preflopOdds)
	{
		vector<int> bestHands;
		auto winningHand = 0;
		cout << "Flop{ " << Card{ flop->cards[0] }.ToString() << ' ' << Card{ flop->cards[1] }.ToString() << ' ' << Card{ flop->cards[2] }.ToString() << " } ";
		cout << "Turn{ " << Card{ turn->cards[0] }.ToString() << " } ";
		cout << "River{ " << Card{ river->cards[0] }.ToString() << " }\n";
		for (auto index = 0; index < players.size(); ++index)
		{
			auto& player = players[index];
			auto bestHand = allHands.GetBestHandRank(player.hole.GetCard1(), player.hole.GetCard2(), flop->cards[0], flop->cards[1], flop->cards[2], turn->cards[0], river->cards[0]);
			bestHands.push_back(bestHand);
			winningHand = ::max(winningHand, bestHand);
		}

		//Compute preflop odds for audience (where we know all players hands)
		Deck remaining;
		remaining.NewDeck();
		for (auto& player : players)
			remaining.Remove(player.hole);
		vector<LargeInteger> preflopAudience;
		preflopAudience.resize(players.size());
		LargeInteger preflopCount = 0;
		auto end = remaining.end();
		for (auto c1 = remaining.begin(); c1 != end; ++c1)
			for (auto c2 = c1 + 1; c2 != end; ++c2)
				for (auto c3 = c2 + 1; c3 != end; ++c3)
					for (auto c4 = c3 + 1; c4 != end; ++c4)
						for (auto c5 = c4 + 1; c5 != end; ++c5)
						{
							++preflopCount;

							vector<int> hands;
							auto best = 0;
							for (auto& player : players)
							{
								auto hand = allHands.GetBestHandRank(player.hole.GetCard1(), player.hole.GetCard2(), *c1, *c2, *c3, *c4, *c5);
								best = ::max(best, hand);
								hands.push_back(hand);
							}

							for (auto index = 0; index < players.size(); ++index)
								if (hands[index] == best)
									++preflopAudience[index];
						}

		//Compute flop odds for the audience
		vector<LargeInteger> flopAudience;
		flopAudience.resize(players.size());
		LargeInteger flopCount = 0;
		deck.Remove(flop->cards[0]);
		deck.Remove(flop->cards[1]);
		deck.Remove(flop->cards[2]);
		end = deck.end();
		for (auto c4 = deck.begin(); c4 != end; ++c4)
			for (auto c5 = c4 + 1; c5 != end; ++c5)
			{
				++flopCount;

				vector<int> hands;
				auto best = 0;
				for (auto& player : players)
				{
					array<int, 5> community{ flop->cards[0], flop->cards[1], flop->cards[2], *c4, *c5 };
					::sort(community.begin(), community.end());
					auto hand = allHands.GetBestHandRank(player.hole.GetCard1(), player.hole.GetCard2(), community[0], community[1], community[2], community[3], community[4]);
					best = ::max(best, hand);
					hands.push_back(hand);
				}

				for (auto index = 0; index < players.size(); ++index)
					if (hands[index] == best)
						++flopAudience[index];
			}

		//Compute turn odds for the audience
		vector<LargeInteger> turnAudience;
		turnAudience.resize(players.size());
		LargeInteger turnCount = 0;
		deck.Remove(turn->cards[0]);
		end = deck.end();
		for (auto c5 = deck.begin(); c5 != end; ++c5)
		{
			++turnCount;

			vector<int> hands;
			auto best = 0;
			for (auto& player : players)
			{
				array<int, 5> community{ flop->cards[0], flop->cards[1], flop->cards[2], turn->cards[0], *c5 };
				::sort(community.begin(), community.end());
				auto hand = allHands.GetBestHandRank(player.hole.GetCard1(), player.hole.GetCard2(), community[0], community[1], community[2], community[3], community[4]);
				best = ::max(best, hand);
				hands.push_back(hand);
			}

			for (auto index = 0; index < players.size(); ++index)
				if (hands[index] == best)
					++turnAudience[index];
		}

		//Compute player odds (not knowing opponents) on river
		vector<LargeOdds> riverPlayer;
		auto opponents = static_cast<int>(players.size()) - 1;
		for (auto& player : players)
		{
			LargeOddsComputer computer;
			array<int, 5> community{ flop->cards[0], flop->cards[1], flop->cards[2], turn->cards[0], river->cards[0] };
			::sort(community.begin(), community.end());
			riverPlayer.push_back(computer.ComputeCommunity(allHands, player.hole, community[0], community[1], community[2], community[3], community[4], opponents));
		}

		auto winningCount = 0;
		for (auto index = 0; index < players.size(); ++index)
		{
			auto& player = players[index];
			auto bestHand = bestHands[index];
			auto preflopToPlayer = preflopOdds.GetOdds(player.hole, static_cast<int>(players.size()) - 1);
			auto preflopToAudience = static_cast<double>((preflopAudience[index] * 10000) / preflopCount) / 100.0;
			auto flopToAudience = static_cast<double>((flopAudience[index] * 10000) / flopCount) / 100.0;
			auto turnToAudience = static_cast<double>((turnAudience[index] * 10000) / turnCount) / 100.0;
			auto riverToPlayer = riverPlayer[index].GetWinOrDrawPercent();
			cout << "Player" << index << " = " << player.hole.ToString()
				<< ", preflop " << preflopToPlayer
				<< " (" << preflopToAudience
				<< "), flop (" << flopToAudience
				<< "), turn (" << turnToAudience
				<< "), river (" << riverToPlayer
				<< ") best hand " << HandValue{ bestHand }.ToString();
			if (bestHand == winningHand)
			{
				++winningCount;
				cout << " (winner)";
			}
			cout << '\n';
		}

		cout << winningCount << " way split.\n";
	}

private:
	Deck deck;
	vector<GamePlayer> players;
	optional<BurnAndTurn> flop;
	optional<BurnAndTurn> turn;
	optional<BurnAndTurn> river;
};

/* TODO: First example takes some time.  Second example took all night.  Need to optimize overlap algorithm.

Flop{ 3c 8h Qs } Turn{ Ts } River{ Th }
Player0 = 4h 7c, preflop 6.68 (8.89), flop (2.76), turn (0), river (0) best hand Pair of Ts, Q87 kicker
Player1 = 9s 9c, preflop 15.44 (10.45), flop (0.92), turn (7.69), river (6.91) best hand Two pair Ts and 9s, Q kicker
Player2 = 7s 8c, preflop 8.9 (7.21), flop (11.38), turn (7.69), river (5.01) best hand Two pair Ts and 8s, Q kicker
Player3 = 2s 8s, preflop 8.46 (13.08), flop (16.92), turn (34.61), river (5.01) best hand Two pair Ts and 8s, Q kicker
Player4 = 2h 4c, preflop 5.91 (6.64), flop (2.76), turn (0), river (0) best hand Pair of Ts, Q84 kicker
Player5 = 6c Tc, preflop 10.56 (21.06), flop (5.84), turn (11.53), river (64.43) best hand Three of kind Ts, Q8 kicker (winner)
Player6 = Qh 5d, preflop 5.62 (10.78), flop (16.61), turn (7.69), river (21.49) best hand Two pair Qs and Ts, 8 kicker
Player7 = Jh Qd, preflop 10.76 (30.83), flop (52.3), turn (34.61), river (26.48) best hand Two pair Qs and Ts, J kicker
Player8 = 9h 9d, preflop 15.44 (9.41), flop (0.92), turn (7.69), river (6.91) best hand Two pair Ts and 9s, Q kicker
1 way split.

Flop{ 2s Tc 3s } Turn{ 8s } River{ Jc }
Player0 = 9h Jd, preflop 9.31 (24.75), flop (12.92), turn (15.38), river (15.46) best hand Pair of Js, T98 kicker (winner)
Player1 = 5h Ad, preflop 6.99 (10.76), flop (21.53), turn (11.53), river (0) best hand High card AJT85
Player2 = As 6c, preflop 6.53 (8.01), flop (12.92), turn (26.92), river (0) best hand High card AJT86
Player3 = 6s Ah, preflop 6.53 (7.59), flop (8.3), turn (3.84), river (0) best hand High card AJT86
Player4 = 3h 7d, preflop 5.52 (9.93), flop (19.38), turn (7.69), river (0.13) best hand Pair of 3s, JT8 kicker
Player5 = 4s 7h, preflop 6.68 (11.88), flop (4), turn (0), river (0) best hand High card JT874
Player6 = Qs 2c, preflop 4.69 (10.13), flop (10.76), turn (3.84), river (0.02) best hand Pair of 2s, QJT kicker
Player7 = 8c Kc, preflop 11.31 (28.68), flop (18.46), turn (34.61), river (1.43) best hand Pair of 8s, KJT kicker
Player8 = Qc 2d, preflop 4.69 (10.1), flop (10.76), turn (3.84), river (0.02) best hand Pair of 2s, QJT kicker
1 way split.
*/

vector<pair<int, int>> CreateTwoCards()
{
	return
	{
		{ 2, 3 }, { 2, 4 }, { 2, 5 }, { 2, 7 }, { 2, 8 }, { 2, 9 }, { 2, 10 }, { 2, 11 }, { 2, 12 },
		{ 3, 4 }, { 3, 5 }, { 3, 7 }, { 3, 8 }, { 3, 9 }, { 3, 10 }, { 3, 11 }, { 3, 12 },
		{ 4, 5 }, { 4, 7 }, { 4, 8 }, { 4, 9 }, { 4, 10 }, { 4, 11 }, { 4, 12 },
		{ 5, 7 }, { 5, 8 }, { 5, 9 }, { 5, 10 }, { 5, 11 }, { 5, 12 }, { 5, 33 }, { 5, 46 },
		{ 7, 8 }, { 7, 9 }, { 7, 10 }, { 7, 11 }, { 7, 12 }, { 7, 18 }, { 7, 23 }, { 7, 31 }, { 7, 36 }, { 7, 44 }, { 7, 49 },
		{ 8, 9 }, { 8, 10 }, { 8, 11 }, { 8, 12 }, { 8, 13 }, { 8, 14 }, { 8, 19 }, { 8, 21 },
		{ 8, 22 }, { 8, 26 }, { 8, 27 }, { 8, 32 }, { 8, 39 }, { 8, 40 }, { 8, 45 }, { 8, 47 },
		{ 9, 10 }, { 9, 11 }, { 9, 12 }, { 9, 13 }, { 9, 14 }, { 9, 19 }, { 9, 21 }, { 9, 22 },
		{ 9, 23 }, { 9, 24 }, { 9, 25 }, { 9, 26 }, { 9, 27 }, { 9, 32 }, { 9, 36 }, { 9, 37 },
		{ 9, 38 }, { 9, 39 }, { 9, 40 }, { 9, 45 }, { 9, 47 }, { 9, 49 }, { 9, 50 }, { 9, 51 },
		{ 10, 11 }, { 10, 12 }, { 10, 22 }, { 10, 23 }, { 10, 33 }, { 10, 36 }, { 10, 46 }, { 10, 49 },
		{ 11, 12 }, { 11, 22 }, { 11, 24 }, { 11, 37 }, { 11, 50 },
		{ 12, 22 }, { 12, 25 }, { 12, 38 }, { 12, 51 },
		{ 13, 14 }, { 13, 19 }, { 13, 21 }, { 13, 22 }, { 13, 26 }, { 13, 27 }, { 13, 32 }, { 13, 39 }, { 13, 40 }, { 13, 45 }, { 13, 47 },
		{ 14, 19 }, { 14, 21 }, { 14, 22 }, { 14, 26 }, { 14, 27 }, { 14, 32 }, { 14, 39 }, { 14, 40 }, { 14, 45 }, { 14, 47 },
		{ 18, 33 }, { 18, 46 },
		{ 19, 21 }, { 19, 22 }, { 19, 26 }, { 19, 27 }, { 19, 32 }, { 19, 39 }, { 19, 40 }, { 19, 45 }, { 19, 47 },
		{ 21, 22 }, { 21, 26 }, { 21, 27 }, { 21, 32 }, { 21, 39 }, { 21, 40 }, { 21, 45 }, { 21, 47 },
		{ 22, 23 }, { 22, 24 }, { 22, 25 }, { 22, 26 }, { 22, 27 }, { 22, 32 }, { 22, 36 }, { 22, 37 },
		{ 22, 38 }, { 22, 39 }, { 22, 40 }, { 22, 45 }, { 22, 47 }, { 22, 49 }, { 22, 50 }, { 22, 51 },
		{ 23, 33 }, { 23, 36 }, { 23, 46 }, { 23, 49 },
		{ 24, 37 }, { 24, 50 },
		{ 25, 38 }, { 25, 51 },
		{ 26, 27 }, { 26, 32 }, { 26, 39 }, { 26, 40 }, { 26, 45 }, { 26, 47 },
		{ 27, 32 }, { 27, 39 }, { 27, 40 }, { 27, 45 }, { 27, 47 },
		{ 31, 33 }, { 31, 46 },
		{ 32, 39 }, { 32, 40 }, { 32, 45 }, { 32, 47 },
		{ 33, 36 }, { 33, 44 }, { 33, 49 },
		{ 36, 46 }, { 36, 49 },
		{ 37, 50 },
		{ 38, 51 },
		{ 39, 40 }, { 39, 45 }, { 39, 47 },
		{ 40, 45 }, { 40, 47 },
		{ 44, 46 },
		{ 45, 47 },
		{ 46, 49 }
	};
}

void Test4()
{
	for (auto opponents = 1; opponents <= 8; ++opponents)
	{
		auto start = std::chrono::system_clock::now();

		auto twoCards = CreateTwoCards();
		vector<HoleCards> holes;
		for (auto& twoCard : twoCards)
			holes.emplace_back(twoCard.first, twoCard.second);
		auto overlap = TwoCardOverlap::Compute(holes, 45, opponents);

		auto lose = ComputeTotalCombinations(43, 2 * opponents - 2) * static_cast<long long>(twoCards.size()) - overlap;
		cout << "Lose: " << lose << " (" << opponents << " opponents)\n";

		auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
		cout << "Duration: " << durationMs << "ms\n";
	}

	//Lose: 196 (1 opponents)
	//Duration: 0ms
	//Lose: 160419 (2 opponents)
	//Duration: 0ms
	//Lose: 59778861 (3 opponents)
	//Duration: 1ms
	//Lose: 13458144328 (4 opponents)
	//Duration: 25ms
	//Lose: 2048339780657 (5 opponents)
	//Duration: 450ms
	//Lose: 223478205275433 (6 opponents)
	//Duration: 7478ms (7s)
	//Lose: 18084284371723401 (7 opponents)
	//Duration: 103488ms (1.7m)
	//Lose: 1108007012317984113 (8 opponents)
	//Duration: 948550ms (15.8m)
}

void Test5()
{
	//Expected Values
	//Per: 15225893865 67445820 246753 666 1
	//Count: 196 16569 800361 24670043 513145502
	//Lose: 2048339780657

	//Actual (correct, but slow)
	//Size: 196
	//Count2: 16569
	//Count3: 800361
	//Count4: 24670043
	//Count5: 513145502
	//Duration: 8245.95ms

	Timer timer;
	auto twoCards = CreateTwoCards();
	using row_t = bitset<DeckSize>;
	cout << "Size of row: " << (sizeof(row_t) * CHAR_BIT) << " bits\n";
	using matrix_t = array<row_t, DeckSize>;
	cout << "Size of matrix: " << sizeof(matrix_t) << " bytes\n";
	matrix_t m;
	for (auto& twoCard : twoCards)
		m[twoCard.first].set(twoCard.second);
	LargeInteger count2 = 0, count3 = 0, count4 = 0, count5 = 0;
	for (auto row1 = 0; row1 < DeckSize; ++row1)
	{
		auto& r1 = m[row1];
		if (r1.none())
			continue;
		for (auto col1 = row1 + 1; col1 < DeckSize; ++col1)
		{
			if (!r1.test(col1))
				continue;
			row_t mask1;
			mask1.flip();
			mask1.set(col1, false);
			for (auto row2 = row1 + 1; row2 < DeckSize; ++row2)
			{
				if (!mask1.test(row2))
					continue;
				auto r2 = m[row2] & mask1;
				if (r2.none())
					continue;
				for (auto col2 = row2 + 1; col2 < DeckSize; ++col2)
				{
					if (!r2.test(col2))
						continue;
					++count2;
					auto mask2 = mask1;
					mask2.set(col2, false);
					for (auto row3 = row2 + 1; row3 < DeckSize; ++row3)
					{
						if (!mask2.test(row3))
							continue;
						auto r3 = m[row3] & mask2;
						if (r3.none())
							continue;
						for (auto col3 = row3 + 1; col3 < DeckSize; ++col3)
						{
							if (!r3.test(col3))
								continue;
							++count3;
							auto mask3 = mask2;
							mask3.set(col3, false);
							for (auto row4 = row3 + 1; row4 < DeckSize; ++row4)
							{
								if (!mask3.test(row4))
									continue;
								auto r4 = m[row4] & mask3;
								if (r4.none())
									continue;
								for (auto col4 = row4 + 1; col4 < DeckSize; ++col4)
								{
									if (!r4.test(col4))
										continue;
									++count4;
									auto mask4 = mask3;
									mask4.set(col4, false);
									for (auto row5 = row4 + 1; row5 < DeckSize; ++row5)
									{
										if (!mask4.test(row5))
											continue;
										auto r5 = m[row5] & mask4;
										count5 += r5.count();
									}
								}
							}
						}
					}
				}
			}
		}
	}

	cout << "Size: " << twoCards.size() << '\n';
	cout << "Count2: " << count2 << '\n';
	cout << "Count3: " << count3 << '\n';
	cout << "Count4: " << count4 << '\n';
	cout << "Count5: " << count5 << '\n';
	cout << "Duration: " << timer.GetDurationMs() << "ms\n";
}

void Test6()
{
	//Size: 196
	//Count2: 16569
	//Count3: 800361
	//Count4: 24670043
	//Count5: 513145502
	//Duration: 3313.01ms

	Timer timer;
	auto twoCards = CreateTwoCards();
	LargeInteger count2 = 0, count3 = 0, count4 = 0, count5 = 0;

	struct row_t
	{
		int value;
		vector<int> columns;
	};
	using matrix_t = vector<row_t>;
	map<int, int> rowIndexByValue;
	matrix_t m;
	for (auto& twoCard : twoCards)
	{
		auto iter = rowIndexByValue.find(twoCard.first);
		if (iter == rowIndexByValue.end())
		{
			rowIndexByValue.emplace(twoCard.first, static_cast<int>(m.size()));
			m.push_back({ twoCard.first, { twoCard.second } });
		}
		else
			m[iter->second].columns.push_back(twoCard.second);
	}
	sort(execution::par_unseq, m.begin(), m.end(), [](const row_t& lhs, const row_t& rhs) -> bool
	{
		return (lhs.columns.size() > rhs.columns.size()) ||
			(lhs.columns.size() == rhs.columns.size() && lhs.value < rhs.value);
	});
	using bits_t = bitset<DeckSize>;
	for (auto row1 = 0; row1 < m.size(); ++row1)
	{
		auto& r1 = m[row1];
		for (auto col1 : r1.columns)
		{
			bits_t b1;
			b1.set(r1.value);
			b1.set(col1);
			for (auto row2 = row1 + 1; row2 < m.size(); ++row2)
			{
				auto& r2 = m[row2];
				if (b1.test(r2.value))
					continue;
				for (auto col2 : r2.columns)
				{
					if (b1.test(col2))
						continue;
					auto b2 = b1;
					b2.set(r2.value);
					b2.set(col2);
					++count2;
					for (auto row3 = row2 + 1; row3 < m.size(); ++row3)
					{
						auto& r3 = m[row3];
						if (b2.test(r3.value))
							continue;
						for (auto col3 : r3.columns)
						{
							if (b2.test(col3))
								continue;
							auto b3 = b2;
							b3.set(r3.value);
							b3.set(col3);
							++count3;
							for (auto row4 = row3 + 1; row4 < m.size(); ++row4)
							{
								auto& r4 = m[row4];
								if (b3.test(r4.value))
									continue;
								for (auto col4 : r4.columns)
								{
									if (b3.test(col4))
										continue;
									auto b4 = b3;
									b4.set(r4.value);
									b4.set(col4);
									++count4;
									for (auto row5 = row4 + 1; row5 < m.size(); ++row5)
									{
										auto& r5 = m[row5];
										if (b4.test(r5.value))
											continue;
										for (auto col5 : r5.columns)
										{
											if (b4.test(col5))
												continue;
											++count5;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	cout << "Size: " << twoCards.size() << '\n';
	cout << "Count2: " << count2 << '\n';
	cout << "Count3: " << count3 << '\n';
	cout << "Count4: " << count4 << '\n';
	cout << "Count5: " << count5 << '\n';
	cout << "Duration: " << timer.GetDurationMs() << "ms\n";
}

void Test7()
{
	//Size: 196
	//Count2: 16569
	//Count3: 800361
	//Count4: 24670043
	//Count5: 513145502
	//Duration: 1607.49ms

	Timer timer;
	auto twoCards = CreateTwoCards();

	struct row_t
	{
		int value;
		vector<int> columns;
		LargeInteger count2, count3, count4, count5;
	};
	using matrix_t = vector<row_t>;
	map<int, int> rowIndexByValue;
	matrix_t m;
	for (auto& twoCard : twoCards)
	{
		auto iter = rowIndexByValue.find(twoCard.first);
		if (iter == rowIndexByValue.end())
		{
			rowIndexByValue.emplace(twoCard.first, static_cast<int>(m.size()));
			m.push_back({ twoCard.first, { twoCard.second }, 0, 0, 0, 0 });
		}
		else
			m[iter->second].columns.push_back(twoCard.second);
	}
	sort(execution::par_unseq, m.begin(), m.end(), [](const row_t& lhs, const row_t& rhs) -> bool
	{
		return (lhs.columns.size() > rhs.columns.size()) ||
			(lhs.columns.size() == rhs.columns.size() && lhs.value < rhs.value);
	});
	using bits_t = bitset<DeckSize>;
	vector<int> rowIndexes(m.size());
	iota(rowIndexes.begin(), rowIndexes.end(), 0);
	for_each(execution::par_unseq, rowIndexes.begin(), rowIndexes.end(), [&](int row1)
	{
		auto& r1 = m[row1];
		for (auto col1 : r1.columns)
		{
			bits_t b1;
			b1.set(r1.value);
			b1.set(col1);
			for (auto row2 = row1 + 1; row2 < m.size(); ++row2)
			{
				auto& r2 = m[row2];
				if (b1.test(r2.value))
					continue;
				for (auto col2 : r2.columns)
				{
					if (b1.test(col2))
						continue;
					auto b2 = b1;
					b2.set(r2.value);
					b2.set(col2);
					++r1.count2;
					for (auto row3 = row2 + 1; row3 < m.size(); ++row3)
					{
						auto& r3 = m[row3];
						if (b2.test(r3.value))
							continue;
						for (auto col3 : r3.columns)
						{
							if (b2.test(col3))
								continue;
							auto b3 = b2;
							b3.set(r3.value);
							b3.set(col3);
							++r1.count3;
							for (auto row4 = row3 + 1; row4 < m.size(); ++row4)
							{
								auto& r4 = m[row4];
								if (b3.test(r4.value))
									continue;
								for (auto col4 : r4.columns)
								{
									if (b3.test(col4))
										continue;
									auto b4 = b3;
									b4.set(r4.value);
									b4.set(col4);
									++r1.count4;
									for (auto row5 = row4 + 1; row5 < m.size(); ++row5)
									{
										auto& r5 = m[row5];
										if (b4.test(r5.value))
											continue;
										for (auto col5 : r5.columns)
										{
											if (b4.test(col5))
												continue;
											++r1.count5;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	});

	LargeInteger count2 = 0, count3 = 0, count4 = 0, count5 = 0;
	for (auto& r : m)
	{
		count2 += r.count2;
		count3 += r.count3;
		count4 += r.count4;
		count5 += r.count5;
	}

	cout << "Size: " << twoCards.size() << '\n';
	cout << "Count2: " << count2 << '\n';
	cout << "Count3: " << count3 << '\n';
	cout << "Count4: " << count4 << '\n';
	cout << "Count5: " << count5 << '\n';
	cout << "Duration: " << timer.GetDurationMs() << "ms\n";
}

HoleCards MakeHole(int c1, int c2)
{
	auto f1 = static_cast<Face>(c1);
	auto f2 = static_cast<Face>(c2);
	if (c1 == c2)
		return { { f1, Suit::Spades }, { f2, Suit::Hearts } };
	if (c1 < c2)
		return { { f1, Suit::Spades }, { f2, Suit::Spades } };
	return { { f2, Suit::Spades }, { f1, Suit::Hearts } };
}

//268 (84 suited and offsuit)
void ComputeMaxTwoCardLossCount()
{
	AllHands allHands;
	auto maxOfAll = 0;
	for (auto c1 = 0; c1 < FaceCount; ++c1)
	{
		for (auto c2 = 0; c2 < FaceCount; ++c2)
		{
			auto hole = MakeHole(c1, c2);
			auto maxOfHand = 0;
			Timer timer;

			Deck deck;
			deck.NewDeck();
			deck.Remove(hole);
			auto begin = deck.begin();
			auto end = deck.end();
			for (auto m1 = begin + 4; m1 != end; ++m1)
				for (auto m2 = begin + 3; m2 != m1; ++m2)
					for (auto m3 = begin + 2; m3 != m2; ++m3)
						for (auto m4 = begin + 1; m4 != m3; ++m4)
						{
							vector<int> sizes(m4 - begin, 0);
							transform(execution::par_unseq, begin, m4, sizes.begin(), [&](int m5) -> int
							{
								Deck remaining;
								remaining.NewDeck();
								remaining.Remove(hole, *m1, *m2, *m3, *m4, m5);
								auto bestHand = allHands.GetBestHandRank(hole.GetCard1(), hole.GetCard2(), *m1, *m2, *m3, *m4, m5);
								remaining.RemoveSingleCardLosses(allHands, bestHand, *m1, *m2, *m3, *m4, m5);
								auto twoCards = remaining.FindTwoCardLosses(allHands, bestHand, *m1, *m2, *m3, *m4, m5);
								return static_cast<int>(twoCards.size());
							});
							auto maxOfCard4 = reduce(execution::par_unseq, sizes.begin(), sizes.end(), 0, [](int x, int y){ return max(x, y); });
							maxOfHand = max(maxOfHand, maxOfCard4);
						}

			cout << hole.ToHandString() << " max 2-card loss count: " << maxOfHand << ", duration "<< timer.GetDurationMs() << "ms\n";
			maxOfAll = max(maxOfAll, maxOfHand);
		}
	}
	cout << "Final max 2-card loss count: " << maxOfAll << '\n';
}

int main(int argc, char** argv)
{
	try
	{
		//Test4();
		//Test5();
		//Test6();
		Test7();
		return 0;

		AllHands allHands;
		PreflopOdds preflopOdds;

		for (;;)
		{
			Game game;
			for (auto player = 0; player < 9; ++player)
				game.AddPlayer();
			game.DealPlayers();
			game.DealFlop();
			game.DealTurn();
			game.DealRiver();

			game.Dump(allHands, preflopOdds);

			cout << "Type exit to quit: ";
			string line;
			getline(cin, line);
			if (line == "exit")
				break;
			cout << '\n';
		}
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
