#include "pch.h"
#include "PreflopOdds.h"

PreflopOdds::PreflopOdds()
{
	ifstream in{ "all-odds.txt" };
	string line;
	getline(in, line);
	while (getline(in, line))
	{
		int c1 = 0, c2 = 0;
		string label, type;
		double o1 = 0, o2 = 0, o3 = 0, o4 = 0, o5 = 0, o6 = 0, o7 = 0, o8 = 0;
		char comma = ',';
		istringstream parts{ line };
		parts >> c1 >> comma >> c2 >> comma;
		getline(parts, label, ',');
		getline(parts, type, ',');
		parts >> o1 >> comma >> o2 >> comma >> o3 >> comma >> o4 >> comma >> o5 >> comma >> o6 >> comma >> o7 >> comma >> o8;
		map<int, double> handOdds{ { 1, o1 }, { 2, o2 }, { 3, o3 }, { 4, o4 }, { 5, o5 }, { 6, o6 }, { 7, o7 }, { 8, o8 } };
		Card card1{ c1 };
		Card card2{ c2 };
		if (type == "pair")
		{
			for (auto suit1 = 0; suit1 < SuitCount; ++suit1)
				for (auto suit2 = suit1 + 1; suit2 < SuitCount; ++suit2)
					odds[HoleCards{ Card{ card1.GetFace(), AllSuits[suit1] }, Card{ card2.GetFace(), AllSuits[suit2] } }.GetHash()] = handOdds;
		}
		else if (type == "suited")
		{
			for (auto suit : AllSuits)
				odds[HoleCards{ Card{ card1.GetFace(), suit }, Card{ card2.GetFace(), suit } }.GetHash()] = handOdds;
		}
		else if (type == "offsuit")
		{
			for (auto suit1 = 0; suit1 < SuitCount; ++suit1)
				for (auto suit2 = 0; suit2 < SuitCount; ++suit2)
					if (suit1 != suit2)
						odds[HoleCards{ Card{ card1.GetFace(), AllSuits[suit1] }, Card{ card2.GetFace(), AllSuits[suit2] } }.GetHash()] = handOdds;
		}
		else
			throw runtime_error{ "Unrecognized hand type: " + type };
	}
	cout << "Preflop odds loaded: " << odds.size() << '\n';
}

double PreflopOdds::GetOdds(const HoleCards& hole, int opponents) const
{
	if (opponents < 1 || opponents > 8)
		throw runtime_error{ "Only support opponents 1 through 8, not: " + to_string(opponents) };
	auto iter = odds.find(hole.GetHash());
	if (iter == odds.end())
		throw runtime_error{ "Missing odds: " + hole.ToString() };
	return iter->second.find(opponents)->second;
}
