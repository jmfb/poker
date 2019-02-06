#include "pch.h"
#include "AllHands.h"
#include "HoleCards.h"
#include "Card.h"
#include "Suit.h"
#include "Face.h"
#include "LargeOddsComputer.h"

int main(int argc, char** argv)
{
	try
	{
		//return LargeOddsComputer::Compute(argc, argv);
		map<pair<int, int>, map<int, LargeOdds>> allOdds;
		for (auto opponents = 1; opponents <= 8; ++opponents)
		{
			ifstream in{ to_string(opponents) + "-opponent-odds.txt" };
			string line;
			while (getline(in, line))
			{
				istringstream parts{ line };
				int card1 = 0, card2 = 0;
				char comma;
				string name;
				LargeInteger winOrDraw = 0, lose = 0;
				parts >> card1 >> comma >> card2 >> comma;
				getline(parts, name, ',');
				parts >> winOrDraw >> comma >> lose;
				allOdds[{ card1, card2 }][opponents] = { winOrDraw, lose };
			}
		}

		ofstream out{ "all-odds.txt" };
		out << "hand,1,2,3,4,5,6,7,8\n";
		for (auto& hand : allOdds)
		{
			out << HoleCards{ { hand.first.first }, { hand.first.second } }.ToHandString();
			for (auto& odds : hand.second)
				out << ',' << odds.second.GetWinOrDrawPercent();
			out << '\n';
		}
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
