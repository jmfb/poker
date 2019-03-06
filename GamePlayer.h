#pragma once
#include "Deck.h"
#include "HoleCards.h"

class GamePlayer
{
public:
	GamePlayer() = default;
	GamePlayer(const GamePlayer& rhs) = default;
	GamePlayer(GamePlayer&& rhs) = default;
	~GamePlayer() = default;

	GamePlayer& operator=(const GamePlayer& rhs) = default;
	GamePlayer& operator=(GamePlayer&& rhs) = default;

	void SetCard1(Deck& deck);
	void SetCard2(Deck& deck);
	const HoleCards& GetHole() const;

private:
	HoleCards hole;
};
