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

		auto winningCount = 0;
		for (auto index = 0; index < players.size(); ++index)
		{
			auto& player = players[index];
			auto bestHand = bestHands[index];
			auto preflopToPlayer = preflopOdds.GetOdds(player.hole, static_cast<int>(players.size()) - 1);
			auto preflopToAudience = static_cast<double>((preflopAudience[index] * 10000) / preflopCount) / 100.0;
			auto flopToAudience = static_cast<double>((flopAudience[index] * 10000) / flopCount) / 100.0;
			auto turnToAudience = static_cast<double>((turnAudience[index] * 10000) / turnCount) / 100.0;
			cout << "Player" << index << " = " << player.hole.ToString()
				<< ", preflop " << preflopToPlayer
				<< " (" << preflopToAudience
				<< "), flop (" << flopToAudience
				<< "), turn (" << turnToAudience
				<< "), best hand "
				<< HandValue{ bestHand }.ToString();
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

int main(int argc, char** argv)
{
	try
	{
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
