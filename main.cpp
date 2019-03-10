#include "pch.h"
#include "AllHands.h"
#include "LargeOddsComputer.h"
#include "PreflopOdds.h"
#include "Timer.h"
#include "CombinationSequences.h"
#include "LargeOddsCache.h"
#include "Game.h"

int main(int argc, char** argv)
{
	try
	{
		//for (auto opponents = 1; opponents <= 5; ++opponents)
		//{
		//	ofstream out{ to_string(opponents) + "-opponent-analysis.txt" };
		//	LargeOddsCache{ opponents }.Analyze(out);
		//}
		//return 0;

		Timer sequenceTimer;
		CombinationSequences::Get(0);
		cout << "Combination sequence generation: " << sequenceTimer.GetDurationMs() << "ms\n";

		//LargeOddsComputer::Compute(argc, argv);
		//return 0;

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
