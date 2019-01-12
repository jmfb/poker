#include "Odds.h"
#include "Deck.h"
#include <sstream>
using namespace std;

Odds::Odds(int win, int draw, int lose)
	: win{ win }, draw{ draw }, lose{ lose }
{
}

void Odds::Compute(const AllHands& allHands, const HoleCards& hole1, const HoleCards& hole2)
{
	for (auto m1 = 0; m1 < DeckSize; ++m1)
	{
		if (hole1.Contains(m1) || hole2.Contains(m1))
			continue;
		for (auto m2 = m1 + 1; m2 < DeckSize; ++m2)
		{
			if (hole1.Contains(m2) || hole2.Contains(m2))
				continue;
			for (auto m3 = m2 + 1; m3 < DeckSize; ++m3)
			{
				if (hole1.Contains(m3) || hole2.Contains(m3))
					continue;
				for (auto m4 = m3 + 1; m4 < DeckSize; ++m4)
				{
					if (hole1.Contains(m4) || hole2.Contains(m4))
						continue;
					for (auto m5 = m4 + 1; m5 < DeckSize; ++m5)
					{
						if (hole1.Contains(m5) || hole2.Contains(m5))
							continue;
						auto leftRank = allHands.GetBestHandRank(hole1.GetCard1(), hole1.GetCard2(), m1, m2, m3, m4, m5);
						auto rightRank = allHands.GetBestHandRank(hole2.GetCard1(), hole2.GetCard2(), m1, m2, m3, m4, m5);
						if (leftRank == rightRank)
							++draw;
						else if (leftRank > rightRank)
							++win;
						else
							++lose;
					}
				}
			}
		}
	}
}

int Odds::GetWin() const
{
	return win;
}

int Odds::GetDraw() const
{
	return draw;
}

int Odds::GetLose() const
{
	return lose;
}

Odds Odds::Invert() const
{
	return { lose, draw, win };
}

string Odds::ToString() const
{
	ostringstream out;
	out << "Win " << win << ", Draw " << draw << ", Lose " << lose;
	return out.str();
}
