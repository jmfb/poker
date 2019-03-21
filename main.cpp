#include "pch.h"
#include "AllHands.h"
#include "LargeOddsComputer.h"
#include "PreflopOdds.h"
#include "Timer.h"
#include "CombinationSequences.h"
#include "LargeOddsCache.h"
#include "Game.h"
#include "HandValue.h"

void SimulateGames()
{
	Timer sequenceTimer;
	CombinationSequences::Get(0);
	cout << "Combination sequence generation: " << sequenceTimer.GetDurationMs() << "ms\n";

	AllHands allHands;
	PreflopOdds preflopOdds;

	for (;;)
	{
		Game game;
		for (auto player = 0; player < 6; ++player)
			game.AddPlayer();
		game.DealPlayers();
		game.DealFlop();
		game.DealTurn();
		game.DealRiver();
		game.ComputeWinningHand(allHands);

		game.Dump(allHands, preflopOdds);

		cout << "Type exit to quit: ";
		string line;
		getline(cin, line);
		if (line == "exit" || line == "quit")
			break;
		cout << "\n\n\n\n";
	}
}

enum class ActionType
{
	FoldMuck,
	FoldShow,
	Check,
	Call,
	CallAllIn,
	Bet,
	BetAllIn,
	Raise,
	RaiseAllIn
};

class Action
{
public:
	Action() = default;
	Action(ActionType type, int callAmount = 0, int raiseAmount = 0)
		: type{ type }, callAmount{ callAmount }, raiseAmount{ raiseAmount }
	{
	}
	Action(const Action& rhs) = default;
	Action(Action&& rhs) = default;
	~Action() = default;

	Action& operator=(const Action& rhs) = default;
	Action& operator=(Action&& rhs) = default;

	static Action Parse(const string& value)
	{
		istringstream in{ value };
		string type;
		in >> type;
		if (type == "muck")
			return { ActionType::FoldMuck };
		if (type == "fold")
			return { ActionType::FoldShow };
	}

private:
	ActionType type = ActionType::FoldMuck;
	int callAmount = 0;
	int raiseAmount = 0;
};

class GameState
{
public:
	bool IsValid(const Action& action) const
	{
		//TODO:
		return true;
	}
};

class PlayerLogic
{
public:
	virtual Action TakeAction(const GameState& state) = 0;
};

class InteractivePlayerLogic : public PlayerLogic
{
public:
	Action TakeAction(const GameState& state) override
	{
		for (;;)
		{
			cout << "Action: ";
			string value;
			getline(cin, value);
			auto action = Action::Parse(value);
			if (state.IsValid(action))
				return action;
			cout << "Invalid action.\n";
		}
	}
};

class AlwaysCallPlayerLogic : public PlayerLogic
{
public:
	Action TakeAction(const GameState& state) override
	{
		//TODO: determine current bet state and remaining chips; then check/call/call all-in
		Action action{ ActionType::Check };
		if (!state.IsValid(action))
			throw runtime_error{ "Oops.  Invalid action logic." };
		return action;
	}
};

class FoldToAnyBetPlayerLogic : public PlayerLogic
{
public:
	Action TakeAction(const GameState& state) override
	{
		//TODO: determine current bet state and either check or fold
		Action action{ ActionType::FoldMuck };
		if (!state.IsValid(action))
			throw runtime_error{ "Oops.  Invalid action logic." };
		return action;
	}
};

int main(int argc, char** argv)
{
	try
	{
		AllHands allHands;

		Deck deck;
		deck.NewDeck();
		deck.Shuffle();
		//Note, initial state of game is random shuffle of deck and player positions.
		array<HoleCards, 3> holes;
		//Small and big blinds get posted, in addition to antes for players sitting.
		array<int, 3> playerChips{ 1000, 1000, 1000 };
		int pot = 0;
		pot += 50;
		playerChips[0] -= 50;
		cout << "Player0 posts small blind: 50\n";
		pot += 100;
		playerChips[1] -= 100;
		cout << "Player1 posts large blind: 100\n";
		//Next, two cards are dealt one at a time to all players
		for (auto card = 0; card < 2; ++card)
			for (auto& hole : holes)
				(hole.*(card == 0 ? &HoleCards::SetCard1 : &HoleCards::SetCard2))(deck.DealCard());
		//Pretend to be player[0] for excitement!
		cout << "Your (player0) cards: " << holes[0].ToString() << '\n';
		//Action then commences to the left of the big bind for first round
		//Player[2] folds!
		cout << "Player2 folds, shows: " << holes[2].ToString() << '\n';
		//Player[0] raises, doubles the blinds.
		playerChips[0] -= 150;
		pot += 150;
		cout << "Player0 (you) raises bet to 200\n";
		//Player[1] calls 100 raise.
		playerChips[1] -= 100;
		pot += 100;
		cout << "Player1 calls.\n";
		cout << "Pot is good: " << pot << '\n';
		//When preflop betting is done, we move to the flop (burn one, turn three)
		BurnAndTurn flop{ deck, 3 };
		cout << "Flop: " << flop.ToString() << '\n';
		//Action on remaining player left of dealer
		cout << "Player0 bets 100!\n";
		playerChips[0] -= 100;
		pot += 100;
		cout << "Player1 calls.\n";
		playerChips[1] -= 100;
		pot += 100;
		cout << "Pot is good: " << pot << '\n';
		BurnAndTurn turn{ deck, 1 };
		cout << "Turn: " << turn.ToString() << '\n';
		cout << "Check/check.  Free river.\n";
		BurnAndTurn river{ deck, 1 };
		cout << "River: " << river.ToString() << '\n';
		Community community{ flop, turn, river };
		auto bestHand = allHands.GetBestHandRank(holes[0], community);
		cout << "Your best hand: " << HandValue{ bestHand }.ToString() << '\n';
		cout << "You bet the pot!\n";
		auto bet = pot;
		playerChips[0] -= bet;
		pot += bet;
		cout << "Call!  Time for a showdown.\n";
		playerChips[1] -= bet;
		pot += bet;
		cout << "Final Pot: " << pot << '\n';
		cout << "Opponents cards: " << holes[1].ToString() << '\n';
		auto opponentHand = allHands.GetBestHandRank(holes[1], community);
		cout << "Opponent hand: " << HandValue{ opponentHand }.ToString() << '\n';
		if (bestHand > opponentHand)
		{
			cout << "You win!\n";
			playerChips[0] += pot;
		}
		else if (bestHand == opponentHand)
		{
			cout << "Draw!\n";
			playerChips[0] += pot / 2;
			playerChips[1] += pot / 2;
			if ((pot % 2) == 1)
			{
				cout << "Extra chip goes to worst position (player0)\n";
				++playerChips[0];
			}
		}
		else
		{
			cout << "You lose!\n";
			playerChips[1] += pot;
		}
		pot = 0;
	}
	catch (const compute::program_build_failure& failure)
	{
		cout << failure.build_log() << '\n';
		return -1;
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
