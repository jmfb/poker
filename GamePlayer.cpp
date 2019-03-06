#include "pch.h"
#include "GamePlayer.h"

void GamePlayer::SetCard1(Deck& deck)
{
	hole.SetCard1(deck.DealCard());
}

void GamePlayer::SetCard2(Deck& deck)
{
	hole.SetCard2(deck.DealCard());
}

const HoleCards& GamePlayer::GetHole() const
{
	return hole;
}

void GamePlayer::SetBestHand(int rank)
{
	bestHand = rank;
}

int GamePlayer::GetBestHand() const
{
	return bestHand;
}
