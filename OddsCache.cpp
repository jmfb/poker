#include "pch.h"
#include "OddsCache.h"

void OddsCache::AddIfMissing(const AllHands& allHands, const HoleCards& hole1, const HoleCards& hole2)
{
	auto hash1 = hole1.GetHash();
	auto hash2 = hole2.GetHash();
	auto existing = cache.find({ hash1, hash2 });
	if (existing != cache.end())
	{
		cout << "cached\n";
		return;
	}
	Odds odds;
	odds.Compute(allHands, hole1, hole2);
	cache[{ hash1, hash2 }] = odds;
	cache[{ hash2, hash1 }] = odds.Invert();

	cout << hole1.ToString() << " vs. " << hole2.ToString() << ": " << odds.ToString() << '\n';

	PermuteHands(hole1, hole2, odds);
}

void OddsCache::PermuteHands(const HoleCards& hole1, const HoleCards& hole2, const Odds& odds)
{
	Card card1{ hole1.GetCard1() };
	Card card2{ hole1.GetCard2() };
	Card card3{ hole2.GetCard1() };
	Card card4{ hole2.GetCard2() };
	set<Suit> suits;
	suits.insert(card1.GetSuit());
	suits.insert(card2.GetSuit());
	suits.insert(card3.GetSuit());
	suits.insert(card4.GetSuit());
	switch (suits.size())
	{
	case 1:
		PermuteHandsOneSuit(card1, card2, card3, card4, odds, suits);
		break;
	case 2:
		PermuteHandsTwoSuits(card1, card2, card3, card4, odds, suits);
		break;
	case 3:
		PermuteHandsThreeSuits(card1, card2, card3, card4, odds, suits);
		break;
	case 4:
		PermuteHandsFourSuits(card1, card2, card3, card4, odds);
		break;
	default:
		throw runtime_error{ "Too many suits." };
	}
}

void OddsCache::PermuteHandsOneSuit(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const set<Suit>& suits)
{
	auto existingSuit = *suits.begin();
	for (auto suit : { Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds })
	{
		if (suit == existingSuit)
			continue;
		map<Suit, Suit> suitMap{ { existingSuit, suit } };
		PermuteSuits(card1, card2, card3, card4, odds, suitMap);
	}
}

void OddsCache::PermuteHandsTwoSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const set<Suit>& suits)
{
	auto existingSuit1 = *suits.begin();
	auto existingSuit2 = *next(suits.begin());
	for (auto suit1 : { Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds })
	{
		for (auto suit2 : { Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds })
		{
			if (suit1 == existingSuit1 && suit2 == existingSuit2 || suit1 == suit2)
				continue;
			map<Suit, Suit> suitMap{ { existingSuit1, suit1 }, { existingSuit2, suit2 } };
			PermuteSuits(card1, card2, card3, card4, odds, suitMap);
		}
	}
}

void OddsCache::PermuteHandsThreeSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const set<Suit>& suits)
{
	auto existingSuit1 = *suits.begin();
	auto existingSuit2 = *next(suits.begin());
	auto existingSuit3 = *next(suits.begin(), 2);
	for (auto suit1 : { Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds })
	{
		for (auto suit2 : { Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds })
		{
			if (suit2 == suit1)
				continue;
			for (auto suit3 : { Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds })
			{
				if (suit3 == suit1 || suit3 == suit2 || suit1 == existingSuit1 && suit2 == existingSuit2 && suit3 == existingSuit3)
					continue;
				map<Suit, Suit> suitMap
				{
					{ existingSuit1, suit1 },
					{ existingSuit2, suit2 },
					{ existingSuit3, suit3 }
				};
				PermuteSuits(card1, card2, card3, card4, odds, suitMap);
			}
		}
	}
}

void OddsCache::PermuteHandsFourSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds)
{
	for (auto suit1 : { Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds })
	{
		for (auto suit2 : { Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds })
		{
			if (suit2 == suit1)
				continue;
			for (auto suit3 : { Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds })
			{
				if (suit3 == suit1 || suit3 == suit2)
					continue;
				for (auto suit4 : { Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds })
				{
					if (suit4 == suit1 || suit4 == suit2 || suit4 == suit3)
						continue;
					if (suit1 == Suit::Spades && suit2 == Suit::Hearts && suit3 == Suit::Clubs && suit4 == Suit::Diamonds)
						continue;
					map<Suit, Suit> suitMap
					{
						{ Suit::Spades, suit1 },
						{ Suit::Hearts, suit2 },
						{ Suit::Clubs, suit3 },
						{ Suit::Diamonds, suit4 }
					};
					PermuteSuits(card1, card2, card3, card4, odds, suitMap);
				}
			}
		}
	}
}

void OddsCache::PermuteSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const map<Suit, Suit>& suitMap)
{
	HoleCards hole1{ card1, card2, suitMap };
	HoleCards hole2{ card3, card4, suitMap };
	auto hash1 = hole1.GetHash();
	auto hash2 = hole2.GetHash();
	cache[{ hash1, hash2 }] = odds;
	cache[{ hash2, hash1 }] = odds.Invert();
}

void OddsCache::Load()
{
	ifstream in{ R"(c:\save\temp\poker.csv)" };
	string line;
	while (getline(in, line))
	{
		istringstream parts{ line };
		char sep = ' ';
		auto hash1 = 0;
		auto hash2 = 0;
		auto win = 0;
		auto draw = 0;
		auto lose = 0;
		parts >> hash1 >> sep >> hash2 >> sep >> win >> sep >> draw >> sep >> lose;
		cache[{ hash1, hash2 }] = { win, draw, lose };
	}
	cout << cache.size() << " cached odds loaded from disk.\n";
}

void OddsCache::Save() const
{
	ofstream out{ R"(c:\save\temp\poker.csv)" };
	for (auto& item : cache)
		out << get<0>(item.first) << ','
		<< get<1>(item.first) << ','
		<< item.second.GetWin() << ','
		<< item.second.GetDraw() << ','
		<< item.second.GetLose() << endl;
}

void OddsCache::Compute(const AllHands& allHands)
{
	Load();
	auto totalCount = 1326 * 1225;
	auto index = 0;
	for (HoleCards hole1; hole1.IsValid(); hole1.MoveNext())
	{
		for (HoleCards hole2; hole2.IsValid(); hole2.MoveNext())
		{
			if (!hole1.IsDisjoint(hole2))
				continue;
			auto percentage = (index * 100) / totalCount;
			cout << index << " of " << totalCount << " (" << percentage << "%): ";
			AddIfMissing(allHands, hole1, hole2);
			++index;
		}
	}
	cout << "Done (saving)!\n";
	Save();
}
