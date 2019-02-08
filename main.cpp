#include "pch.h"
#include "AllHands.h"
#include "HoleCards.h"
#include "Card.h"
#include "Suit.h"
#include "Face.h"
#include "LargeOddsComputer.h"
#include "HandValue.h"
#include "PreflopOdds.h"

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

#include "Math.h"

LargeInteger ComputeTwoCardOverlap(
	const CardSet& holes,
	vector<HoleCards>::const_iterator begin,
	vector<HoleCards>::const_iterator end,
	LargeInteger remaining,
	LargeInteger opponentCards)
{
	if (opponentCards <= 0)
		return 0;

	if (holes.GetSize() > 4)
		return 0; //TODO: This will only approximate the overlap

	auto overlapPerPair = ComputeTotalCombinations(remaining - 2, opponentCards - 2);
	LargeInteger overlap = 0;
	for (auto iter = begin; iter != end; ++iter)
	{
		if (holes.Intersect(*iter))
			continue;
		overlap += overlapPerPair;
		auto subOverlap = ComputeTwoCardOverlap(holes.Union(*iter), begin, iter, remaining - 2, opponentCards - 2);
		overlap -= subOverlap;
	}
	return overlap;
}

LargeOdds ComputeCommunity(
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
	cout << "Remaining cards: " << cards.GetSize() << '\n';
	auto twoCards = cards.FindTwoCardLosses(allHands, bestHand, c1, c2, c3, c4, c5);
	cout << "2-card losses: " << twoCards.size() << '\n';

	//Debugging file for possible overlap computation
	if (false)
	{
		//lose
		//remaining cards
		//c1,c2,name
		//...
		ofstream out{ "two-card-losses.txt" };
		out << "2048339780657\n";
		for (auto card : cards)
			out << card << ' ';
		out << '\n';
		for (auto& twoCard : twoCards)
			out << twoCard.GetCard1() << ',' << twoCard.GetCard2() << ',' << twoCard.ToString() << '\n';
		cout << "DONE!\n";
		exit(0);
	}

	auto opponentCards = opponents * 2;
	auto winOrDraw = ComputeTotalCombinations(cards.GetSize(), opponentCards) -
		ComputeTotalCombinations(cards.GetSize() - 2, opponentCards - 2) * static_cast<long long>(twoCards.size());
	cout << "Initial loss (includes overlap): " << (ComputeTotalCombinations(cards.GetSize() - 2, opponentCards - 2) * static_cast<long long>(twoCards.size())) << '\n';
	LargeInteger totalOverlap = 0;
	cout << "Win or Draw before overlap: " << winOrDraw << '\n';

	vector<LargeInteger> combos;
	for (auto remainingOpponents = opponents - 2; remainingOpponents >= 0; --remainingOpponents)
	{
		auto overlapPer = ComputeTotalCombinations(cards.GetSize() - 2 * (opponents - remainingOpponents), remainingOpponents * 2);
		combos.push_back(overlapPer);
		cout << "Overlap Per " << remainingOpponents << " = " << overlapPer << '\n';
	}
	if (!combos.empty())
	{
		array<array<LargeInteger, 8>, 1326> fuckTonOfData;
		for (auto series = 0; series < 8; ++series)
			for (auto n = 0; n < 1326; ++n)
				fuckTonOfData[n][series] = series == 0 ? 1 : n == 0 ? 0 : fuckTonOfData[n - 1][series] + fuckTonOfData[n - 1][series - 1];

		for (auto index = 0; index < (opponents - 1); ++index)
		{
			auto x = fuckTonOfData[twoCards.size()][index + 2];

			cout << "  Index = " << index << '\n';
			cout << "  Multiplier = " << combos[index] << '\n';
			cout << "  X = " << x << '\n';
			cout << "  Value = " << (x * combos[index]) << '\n';

			totalOverlap += (x * combos[index]);
		}
		cout << "Overlap so far: " << totalOverlap << '\n';

		auto countOverlap = 0;
		CardSet cardsEncountered;
		for (auto& twoCard : twoCards)
		{
			if (cardsEncountered.Intersect(twoCard))
				++countOverlap;
			cardsEncountered.AddCards(twoCard);
		}
		cout << "2-Card Overlap Count: " << countOverlap << '\n';
		cout << "Backing off by: " << (countOverlap * combos.front()) << '\n';
		totalOverlap -= countOverlap * combos.front();
	}


/* The following is correct but slow for large opponents/two-card set combinations.
	for (auto iter = twoCards.begin(); iter != twoCards.end(); ++iter)
	{
		auto overlap = ComputeTwoCardOverlap({ *iter }, twoCards.begin(), iter, cards.GetSize() - 2, opponentCards - 2);
		winOrDraw += overlap;
		totalOverlap += overlap;
	}
*/
	LargeInteger expectedOverlap = 0;
	istringstream{ "935935416883" } >> expectedOverlap;

	cout << "Total Overlap:    " << totalOverlap << '\n';
	cout << "Expected Overlap: " << expectedOverlap << '\n';
	winOrDraw -= totalOverlap;

	auto largeOdds = LargeOdds::Create(winOrDraw, opponents);
	return largeOdds;
}




void Test()
{
	ifstream in{ "two-card-losses.txt" };
	string line;
	
	getline(in, line);
	LargeInteger expectedLose = 0;
	istringstream{ line } >> expectedLose;
	cout << "Expected lose: " << expectedLose << '\n';
	const auto opponents = 8;

	getline(in, line);
	istringstream remainingCards{ line };
	Deck deck;
	for (auto card = 0; remainingCards >> card; )
		deck.Add(card);
	cout << deck.GetSize() << " cards remaining in the deck.\n";

	vector<HoleCards> twoCards;
	while (getline(in, line))
	{
		istringstream cards{ line };
		int card1 = 0, card2 = 0;
		char comma = ',';
		cards >> card1 >> comma >> card2;
		twoCards.emplace_back(card1, card2);
	}
	cout << twoCards.size() << " 2-card losses.\n";

	auto opponentCards = opponents * 2;
	auto total = ComputeTotalCombinations(deck.GetSize(), opponentCards);
	auto lossPerTwoCard = ComputeTotalCombinations(deck.GetSize() - 2, opponentCards - 2);
	auto upperLoss = lossPerTwoCard * static_cast<long long>(twoCards.size());
	LargeInteger totalOverlap = 0;
	for (auto iter = twoCards.begin(); iter != twoCards.end(); ++iter)
		totalOverlap += ComputeTwoCardOverlap({ *iter }, twoCards.begin(), iter, deck.GetSize() - 2, opponentCards - 2);
	auto lose = upperLoss - totalOverlap;
	auto winOrDraw = total - lose;
	LargeOdds odds{ winOrDraw, lose };
	cout << "Calculated Lose: " << odds.GetLose() << '\n'; //Should be same as expectedLose
	cout << "Win or Draw: " << odds.GetWinOrDrawPercent() << '\n'; //Should be 32.05
	cout << (expectedLose == lose ? "Correct" : "Incorrect") << '\n';
}

void Test2()
{
	vector<HoleCards> twoCards
	{
		{ 0, 1 },
		{ 0, 2 },
		{ 1, 2 },
		{ 1, 3 },
		{ 2, 3 },
		{ 2, 4 },
		{ 3, 5 },
		{ 4, 5 },
		{ 6, 7 },
		{ 8, 9 }
	};
	const auto remainingCards = 12;
	const auto opponents = 4;
	const auto opponentCards = opponents * 2;
	cout << twoCards.size() << " 2-card losses.\n";
	cout << remainingCards << " remaining cards.\n";
	cout << opponents << " opponents.\n";
	cout << opponentCards << " opponent cards.\n";

	auto total = ComputeTotalCombinations(remainingCards, opponentCards);
	cout << "Total: " << total << '\n';
	auto lossPerTwoCard = ComputeTotalCombinations(remainingCards - 2, opponentCards - 2);
	cout << "Loss per 2-card: " << lossPerTwoCard << '\n';
	for (auto x = 0; x <= opponents; ++x)
		cout << "CT" << x << " = " << ComputeTotalCombinations(remainingCards - 2 * x, opponentCards - 2 * x) << '\n';
	auto upperLoss = lossPerTwoCard * static_cast<long long>(twoCards.size());
	cout << "Upper: " << upperLoss << '\n';
	LargeInteger totalOverlap = 0;
	for (auto iter = twoCards.begin(); iter != twoCards.end(); ++iter)
	{
		auto overlap = ComputeTwoCardOverlap({ *iter }, twoCards.begin(), iter, remainingCards - 2, opponentCards - 2);
		cout << " " << iter->ToString() << " overlap " << overlap << '\n';
		totalOverlap += overlap;
	}
	cout << "Total Overlap: " << totalOverlap << '\n';
	auto lose = upperLoss - totalOverlap;
	cout << "Lose: " << lose << '\n';
	auto winOrDraw = total - lose;
	cout << "Win or Draw: " << winOrDraw << '\n';

	cout << "Fuck ton data (perpetual sum of series)\n";
	array<array<LargeInteger, 8>, 1326> fuckTonOfData;
	for (auto series = 0; series < 8; ++series)
		for (auto n = 0; n < 1326; ++n)
			fuckTonOfData[n][series] = series == 0 ? 1 : n == 0 ? 0 : fuckTonOfData[n - 1][series] + fuckTonOfData[n - 1][series - 1];
	for (auto pair = 0; pair < 11; ++pair)
	{
		for (auto opp = 0; opp < 4; ++opp)
		{
			cout << fuckTonOfData[pair][opp] << ' ';
		}
		cout << '\n';
	}
	cout << '\n';

	cout << "Previous overlap table:\n";
	for (auto iter = twoCards.begin(); iter != twoCards.end(); ++iter)
	{
		auto count = 0;
		for (auto prev = twoCards.begin(); prev != iter; ++prev)
		{
			if (!prev->IsDisjoint(*iter))
				++count;
		}
		cout << count << '\n';
	}

	/*
	10 2-card losses.
	12 remaining cards.
	4 opponents.
	8 opponent cards.
	Total: 51975
	Loss per 2-card: 3150
	Upper: 31500
	Total Overlap: 5764
	Lose: 25736
	Win or Draw: 26239
	*/
}

int main(int argc, char** argv)
{
	try
	{
		Test();
		//Test2();
		return 0;

		AllHands allHands;
		PreflopOdds preflopOdds;

		LargeInteger expectedLose = 0;
		istringstream{ "2048339780657" } >> expectedLose;

		HoleCards hole{ { Face::Nine, Suit::Hearts }, { Face::Jack, Suit::Diamonds } };
		cout << "Hole: " << hole.ToString() << '\n';
		array<int, 5> community
		{
			Card{ Face::Two, Suit::Spades }.GetId(),
			Card{ Face::Ten, Suit::Clubs }.GetId(),
			Card{ Face::Three, Suit::Spades }.GetId(),
			Card{ Face::Eight, Suit::Spades }.GetId(),
			Card{ Face::Jack, Suit::Clubs }.GetId()
		};
		::sort(community.begin(), community.end());
		//for (auto opponents = 1; opponents < 9; ++opponents)
		const auto opponents = 5;
		{
			cout << "Computing " << opponents << "-opponents odds...";
			auto start = std::chrono::system_clock::now();
			auto odds = ComputeCommunity(allHands, hole, community[0], community[1], community[2], community[3], community[4], opponents);
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
			cout << "done in " << duration.count() << "ms\n";
			cout << "Win or draw: " << odds.GetWinOrDrawPercent() << '\n';
			cout << "Lose: " << odds.GetLose() << '\n';
			cout << "Expected Lose: " << expectedLose << '\n';
			cout << "Total: " << odds.GetTotal() << '\n';

			if (odds.GetLose() != expectedLose)
				cout << "Off by: " << (odds.GetLose() - expectedLose) << '\n';
			else
				cout << "Success!\n";

			/*
			Hole: 9h Jd
			Computing 5-opponents odds...Remaining cards: 45

			2-card losses: 196

			Total:                           3014726985270
			Initial loss (includes overlap): 2984275197540
			Total Overlap:                    935935416883
			Lose:                            2048339780657
			*/
		}

		return 0;

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
