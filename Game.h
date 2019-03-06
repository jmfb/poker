#pragma once
#include "AllHands.h"
#include "PreflopOdds.h"
#include "Deck.h"
#include "GamePlayer.h"
#include "BurnAndTurn.h"

class Game
{
public:
	Game() = default;
	Game(const Game& rhs) = default;
	Game(Game&& rhs) = default;
	~Game() = default;

	Game& operator=(const Game& rhs) = default;
	Game& operator=(Game&& rhs) = default;

	void AddPlayer();
	void DealPlayers();
	void DealFlop();
	void DealTurn();
	void DealRiver();

	void ComputeWinningHand(const AllHands& allHands);
	int GetWinningHand() const;

	void Dump(const AllHands& allHands, const PreflopOdds& preflopOdds) const;

private:
	Deck deck;
	vector<GamePlayer> players;
	optional<BurnAndTurn> flop;
	optional<BurnAndTurn> turn;
	optional<BurnAndTurn> river;
	int winningHand = 0;
};
