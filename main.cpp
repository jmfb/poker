#include "pch.h"
#include "AllHands.h"
#include "HoleCards.h"
#include "Card.h"
#include "Suit.h"
#include "Face.h"
#include "LargeOddsComputer.h"
#include "HandValue.h"

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

	void Dump(const AllHands& allHands)
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

		auto winningCount = 0;
		for (auto index = 0; index < players.size(); ++index)
		{
			auto& player = players[index];
			auto bestHand = bestHands[index];
			cout << "Player" << index << " = " << player.hole.ToString() << ", best hand " << HandValue{ bestHand }.ToString();
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
		for (;;)
		{
			Game game;
			for (auto player = 0; player < 9; ++player)
				game.AddPlayer();
			game.DealPlayers();
			game.DealFlop();
			game.DealTurn();
			game.DealRiver();

			game.Dump(allHands);

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
