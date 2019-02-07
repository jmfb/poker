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

int main(int argc, char** argv)
{
	try
	{
		AllHands allHands;
		PreflopOdds preflopOdds;

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
			LargeOddsComputer computer;
			auto odds = computer.ComputeCommunity(allHands, hole, community[0], community[1], community[2], community[3], community[4], opponents);
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
			cout << "done in " << duration.count() << "ms\n";
			cout << "Win or draw: " << odds.GetWinOrDrawPercent() << '\n';
			cout << "Lose: " << odds.GetLose() << '\n';
			cout << "Total: " << odds.GetTotal() << '\n';
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
