#include "pch.h"
#include "Game.h"
#include "LargeOdds.h"
#include "LargeOddsComputer.h"
#include "HandValue.h"

void Game::AddPlayer()
{
	players.push_back({});
}

void Game::DealPlayers()
{
	deck.NewDeck();
	deck.Shuffle();
	for (auto& player : players)
		player.SetCard1(deck);
	for (auto& player : players)
		player.SetCard2(deck);
}

void Game::DealFlop()
{
	flop = { deck, 3 };
}

void Game::DealTurn()
{
	turn = { deck, 1 };
}

void Game::DealRiver()
{
	river = { deck, 1 };
}

void Game::ComputeWinningHand(const AllHands& allHands)
{
	for (auto& player : players)
	{
		auto bestHand = allHands.GetBestHandRank(player, { *flop, *turn, *river });
		player.SetBestHand(bestHand);
		winningHand = max(winningHand, bestHand);
	}
}

int Game::GetWinningHand() const
{
	return winningHand;
}

void Game::Dump(const AllHands& allHands, const PreflopOdds& preflopOdds) const
{
	cout << "                 Preflop     "
		<< "Flop{ " << flop->ToString() << " } "
		<< "Turn{ " << turn->ToString() << " } "
		<< "River{ " << river->ToString() << " }\n";
	cout << "                 =========== ================ ========== ===========\n";

	//Compute preflop odds for audience (where we know all players hands)
	Deck remaining;
	remaining.NewDeck();
	for (auto& player : players)
		remaining.Remove(player.GetHole());
	vector<uint128_t> preflopAudience;
	preflopAudience.resize(players.size());
	uint128_t preflopCount{ 0 };
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
							auto hand = allHands.GetBestHandRank(player, { *c1, *c2, *c3, *c4, *c5 });
							best = ::max(best, hand);
							hands.push_back(hand);
						}

						for (auto index = 0; index < players.size(); ++index)
							if (hands[index] == best)
								++preflopAudience[index];
					}

	//Compute flop odds for the audience
	vector<uint128_t> flopAudience;
	flopAudience.resize(players.size());
	uint128_t flopCount{ 0 };
	remaining.Remove(flop->GetCard(0));
	remaining.Remove(flop->GetCard(1));
	remaining.Remove(flop->GetCard(2));
	end = remaining.end();
	for (auto c4 = remaining.begin(); c4 != end; ++c4)
		for (auto c5 = c4 + 1; c5 != end; ++c5)
		{
			++flopCount;

			vector<int> hands;
			auto best = 0;
			for (auto& player : players)
			{
				auto hand = allHands.GetBestHandRank(player, { *flop, *c4, *c5 });
				best = ::max(best, hand);
				hands.push_back(hand);
			}

			for (auto index = 0; index < players.size(); ++index)
				if (hands[index] == best)
					++flopAudience[index];
		}

	//Compute turn odds for the audience
	vector<uint128_t> turnAudience;
	turnAudience.resize(players.size());
	uint128_t turnCount{ 0 };
	remaining.Remove(turn->GetCard(0));
	end = remaining.end();
	for (auto c5 = remaining.begin(); c5 != end; ++c5)
	{
		++turnCount;

		vector<int> hands;
		auto best = 0;
		for (auto& player : players)
		{
			auto hand = allHands.GetBestHandRank(player, { *flop, *turn, *c5 });
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
		array<int, 5> community{ flop->GetCard(0), flop->GetCard(1), flop->GetCard(2), turn->GetCard(0), river->GetCard(0) };
		::sort(community.begin(), community.end());
		riverPlayer.push_back(computer.ComputeCommunity(allHands, player.GetHole(), community[0], community[1], community[2], community[3], community[4], opponents));
	}

	auto winningCount = 0;
	for (auto index = 0; index < players.size(); ++index)
	{
		auto& player = players[index];
		auto bestHand = player.GetBestHand();
		//auto preflopToPlayer = preflopOdds.GetOdds(player.GetHole(), static_cast<int>(players.size()) - 1);
		auto preflopToAudience = (static_cast<int>((preflopAudience[index] * 1000) / preflopCount) + 5) / 10;
		auto flopToAudience = (static_cast<int>((flopAudience[index] * 1000) / flopCount) + 5) / 10;
		auto turnToAudience = (static_cast<int>((turnAudience[index] * 1000) / turnCount) + 5) / 10;
		auto riverToPlayer = riverPlayer[index].GetWinOrDrawPercentRounded();

		cout << "Player" << index << "{ " << player.GetHole().ToString() << " } "
			<< "??? (" << setw(3) << setfill(' ') << right << preflopToAudience << ")   "
			<< "??? (" << setw(3) << setfill(' ') << right << flopToAudience << ")        "
			<< "??? (" << setw(3) << setfill(' ') << right << turnToAudience << ")  "
			<< setw(3) << setfill(' ') << right << riverToPlayer
			<< " (" << (bestHand == winningHand ? 'x' : ' ') << ") "
			<< HandValue{ bestHand }.ToString() << '\n';
	}
}
