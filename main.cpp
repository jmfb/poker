#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <string>
#include <sstream>
#include <optional>
#include <stdexcept>
using namespace std;

enum class Suit
{
	Spades = 0,
	Hearts,
	Clubs,
	Diamonds
};
const auto SuitCount = 4;

string ToString(Suit value)
{
	switch (value)
	{
	case Suit::Spades: return "s";
	case Suit::Hearts: return "h";
	case Suit::Clubs: return "c";
	case Suit::Diamonds: return "d";
	}
	throw runtime_error{ "Invalid suit." };
}

enum class Face
{
	Ace = 0,
	Two,
	Three,
	Four,
	Five,
	Six,
	Seven,
	Eight,
	Nine,
	Ten,
	Jack,
	Queen,
	King
};
const auto FaceCount = 13;

string ToString(Face value)
{
	switch (value)
	{
	case Face::Ace: return "A";
	case Face::Two: return "2";
	case Face::Three: return "3";
	case Face::Four: return "4";
	case Face::Five: return "5";
	case Face::Six: return "6";
	case Face::Seven: return "7";
	case Face::Eight: return "8";
	case Face::Nine: return "9";
	case Face::Ten: return "T";
	case Face::Jack: return "J";
	case Face::Queen: return "Q";
	case Face::King: return "K";
	}
	throw new runtime_error{ "Invalid face." };
}

int Compare(Face lhs, Face rhs)
{
	if (lhs == rhs)
		return 0;
	if (lhs == Face::Ace)
		return 1;
	if (rhs == Face::Ace)
		return -1;
	if (static_cast<int>(lhs) < static_cast<int>(rhs))
		return -1;
	return 1;
}

int GetRank(Face value)
{
	return (static_cast<int>(value) + FaceCount - 1) % FaceCount;
}

Face FaceFromRank(int value)
{
	return static_cast<Face>((value + FaceCount + 1) % FaceCount);
}

const auto DeckSize = SuitCount * FaceCount;

class Card
{
public:
	Card() = default;
	Card(const Card& rhs) = default;
	Card(Card&& rhs) = default;
	~Card() = default;

	Card(int id)
	{
		SetId(id);
	}

	Card(Suit suit, Face face)
	{
		Set(suit, face);
	}
	Card(Face face, Suit suit)
	{
		Set(suit, face);
	}

	Card& operator=(const Card& rhs) = default;
	Card& operator=(Card&& rhs) = default;

	void SetId(int id)
	{
		if (id < 0 || id >= DeckSize)
			throw runtime_error{ "Invalid card id." };
		suit = static_cast<Suit>(id / FaceCount);
		face = static_cast<Face>(id % FaceCount);
	}

	int GetId() const
	{
		return static_cast<int>(suit) * FaceCount + static_cast<int>(face);
	}

	void Set(Suit suit, Face face)
	{
		this->suit = suit;
		this->face = face;
	}

	void SetSuit(Suit value)
	{
		suit = value;
	}

	void SetFace(Face value)
	{
		face = value;
	}

	Suit GetSuit() const
	{
		return suit;
	}

	Face GetFace() const
	{
		return face;
	}

	bool operator==(const Card& rhs) const
	{
		return suit == rhs.suit && face == rhs.face;
	}

	bool operator!=(const Card& rhs) const
	{
		return !operator==(rhs);
	}

	string ToString() const
	{
		return ::ToString(face) + ::ToString(suit);
	}

private:
	Suit suit = Suit::Spades;
	Face face = Face::Ace;
};

enum class HandType
{
	HighCard = 0,
	OnePair,
	TwoPair,
	ThreeOfAKind,
	Straight,
	Flush,
	FullHouse,
	FourOfAKind,
	StraightFlush,
	RoyalFlush
};

const auto HandSize = 5;

class HandValue
{
public:
	HandValue() = default;
	HandValue(const HandValue& rhs) = default;
	HandValue(HandValue&& rhs) = default;
	~HandValue() = default;

	HandValue(HandType type, const array<Face, HandSize>& faces)
		: type{ type }, faces{ faces }
	{
	}

	HandValue(int rank)
	{
		SetRank(rank);
	}

	HandValue& operator=(const HandValue& rhs) = default;
	HandValue& operator=(HandValue&& rhs) = default;

	HandType GetType() const
	{
		return type;
	}

	array<Face, HandSize> GetFaces() const
	{
		return faces;
	}

	string ToString() const
	{
		switch (type)
		{
		case HandType::HighCard:
			return "High card " + ::ToString(faces[0]) + ::ToString(faces[1]) + ::ToString(faces[2]) + ::ToString(faces[3]) + ::ToString(faces[4]);
		case HandType::OnePair:
			return "Pair of " + ::ToString(faces[0]) + "s, " + ::ToString(faces[1]) + ::ToString(faces[2]) + ::ToString(faces[3]) + " kicker";
		case HandType::TwoPair:
			return "Two pair " + ::ToString(faces[0]) + "s and " + ::ToString(faces[1]) + "s, " + ::ToString(faces[2]) + " kicker";
		case HandType::ThreeOfAKind:
			return "Three of kind " + ::ToString(faces[0]) + "s, " + ::ToString(faces[1]) + ::ToString(faces[2]) + " kicker";
		case HandType::Straight:
			return "Straight to the " + ::ToString(faces[0]);
		case HandType::Flush:
			return "Flush " + ::ToString(faces[0]) + ::ToString(faces[1]) + ::ToString(faces[2]) + ::ToString(faces[3]) + ::ToString(faces[4]);
		case HandType::FullHouse:
			return "Full house, " + ::ToString(faces[0]) + "s full of " + ::ToString(faces[1]) + "s";
		case HandType::FourOfAKind:
			return "Four of a kind " + ::ToString(faces[0]) + "s, " + ::ToString(faces[1]) + " kicker.";
		case HandType::StraightFlush:
			return "Straight flush to the " + ::ToString(faces[0]);
		case HandType::RoyalFlush:
			return "Royal flush";
		}
		throw runtime_error{ "Invalid hand type." };
	}

	int CompareFaces(const array<Face, HandSize>& other) const
	{
		for (auto index = 0; index < HandSize; ++index)
		{
			auto result = ::Compare(faces[index], other[index]);
			if (result != 0)
				return result;
		}
		return 0;
	}

	bool operator==(const HandValue& rhs) const
	{
		return type == rhs.type && faces == rhs.faces;
	}
	bool operator!=(const HandValue& rhs) const
	{
		return !operator==(rhs);
	}
	bool operator<(const HandValue& rhs) const
	{
		return static_cast<int>(type) < static_cast<int>(rhs.type) || (type == rhs.type && CompareFaces(rhs.faces) < 0);
	}
	bool operator<=(const HandValue& rhs) const
	{
		return !(rhs < *this);
	}
	bool operator>(const HandValue& rhs) const
	{
		return rhs < *this;
	}
	bool operator>=(const HandValue& rhs) const
	{
		return !operator<(rhs);
	}

	int GetRank() const
	{
		auto rank = static_cast<int>(type);
		for (auto face : faces)
		{
			rank *= FaceCount;
			rank += ::GetRank(face);
		}
		return rank;
	}

	void SetRank(int value)
	{
		auto rank = value;
		for (auto& face : faces)
		{
			face = FaceFromRank(rank % FaceCount);
			rank /= FaceCount;
		}
		reverse(faces.begin(), faces.end());
		type = static_cast<HandType>(rank);
	}

private:
	HandType type = HandType::HighCard;
	array<Face, HandSize> faces = {};
};

class Hand
{
public:
	Hand() = default;
	Hand(const Hand& rhs) = default;
	Hand(Hand&& rhs) = default;
	~Hand() = default;

	Hand(int c1, int c2, int c3, int c4, int c5)
	{
		Set(c1, c2, c3, c4, c5);
	}
	Hand(int hash)
	{
		SetHash(hash);
	}

	Hand& operator=(const Hand& rhs) = default;
	Hand& operator=(Hand&& rhs) = default;

	void Set(int c1, int c2, int c3, int c4, int c5)
	{
		if (c2 <= c1 || c3 <= c2 || c4 <= c3 || c5 <= c4)
			throw runtime_error{ "Invalid hand." };
		cards[0].SetId(c1);
		cards[1].SetId(c2);
		cards[2].SetId(c3);
		cards[3].SetId(c4);
		cards[4].SetId(c5);
		value = DetermineValue();
	}

	HandValue DetermineValue() const
	{
		auto highCards = GetHighCards();
		auto straightTo = IsStraightTo(highCards);
		if (IsFlush())
		{
			if (straightTo)
			{
				if (*straightTo == Face::Ace)
					return { HandType::RoyalFlush, { Face::Ace } };
				return { HandType::StraightFlush, { *straightTo } };
			}
			return { HandType::Flush, highCards };
		}

		if (straightTo)
			return { HandType::Straight, { *straightTo } };

		map<Face, int> counts;
		for (auto face : highCards)
			++counts[face];
		vector<tuple<Face, int>> faceByCount;
		for (auto& count : counts)
			faceByCount.emplace_back(count.first, count.second);
		sort(faceByCount.begin(), faceByCount.end(), [](auto& lhs, auto& rhs)
		{
			return get<1>(lhs) > get<1>(rhs) || (get<1>(lhs) == get<1>(rhs) && ::Compare(get<0>(lhs), get<0>(rhs)) > 0);
		});
		switch (faceByCount.size())
		{
		case 5:
			return { HandType::HighCard, highCards };
		case 4:
			return { HandType::OnePair, { get<0>(faceByCount[0]), get<0>(faceByCount[1]), get<0>(faceByCount[2]), get<0>(faceByCount[3]) } };
		case 3:
			return { get<1>(faceByCount[0]) == 3 ? HandType::ThreeOfAKind : HandType::TwoPair, { get<0>(faceByCount[0]), get<0>(faceByCount[1]), get<0>(faceByCount[2]) } };
		case 2:
			return { get<1>(faceByCount[0]) == 4 ? HandType::FourOfAKind : HandType::FullHouse, { get<0>(faceByCount[0]), get<0>(faceByCount[1]) } };
		default:
			throw runtime_error{ "Invalid hand value." };
		}
	}

	const array<Card, HandSize>& GetCards() const
	{
		return cards;
	}

	const HandValue& GetValue() const
	{
		return value;
	}

	string ToString() const
	{
		return cards[0].ToString() + " " +
			cards[1].ToString() + " " +
			cards[2].ToString() + " " +
			cards[3].ToString() + " " +
			cards[4].ToString();
	}

	void SetHash(int value)
	{
		auto hash = value;
		for (auto& card : cards)
		{
			card.SetId(hash % DeckSize);
			hash /= DeckSize;
		}
		reverse(cards.begin(), cards.end());
		this->value = DetermineValue();
	}

	bool operator==(const Hand& rhs) const
	{
		return cards == rhs.cards;
	}
	bool operator!=(const Hand& rhs) const
	{
		return !operator==(rhs);
	}

private:
	bool IsFlush() const
	{
		auto suit = cards[0].GetSuit();
		return suit == cards[1].GetSuit() &&
			suit == cards[2].GetSuit() &&
			suit == cards[3].GetSuit() &&
			suit == cards[4].GetSuit();
	}

	optional<Face> IsStraightTo(const array<Face, HandSize>& highCards) const
	{
		if (highCards[0] == Face::Ace)
		{
			if (highCards == array<Face, HandSize>{ Face::Ace, Face::King, Face::Queen, Face::Jack, Face::Ten })
				return Face::Ace;
			if (highCards == array<Face, HandSize>{ Face::Ace, Face::Five, Face::Four, Face::Three, Face::Two })
				return Face::Five;
			return {};
		}
		auto to = static_cast<int>(highCards[0]);
		for (auto index = 1; index < HandSize; ++index)
			if (static_cast<int>(highCards[index]) != (to - index))
				return {};
		return highCards[0];
	}

	array<Face, HandSize> GetHighCards() const
	{
		array<Face, HandSize> highCards;
		for (auto index = 0; index < HandSize; ++index)
			highCards[index] = cards[index].GetFace();
		sort(highCards.begin(), highCards.end(), [](Face lhs, Face rhs) { return ::Compare(lhs, rhs) > 0; });
		return highCards;
	}

private:
	array<Card, HandSize> cards = {};
	HandValue value;
};

int HandHash(int c1, int c2, int c3, int c4, int c5)
{
	auto hash = c1;
	hash *= DeckSize;
	hash += c2;
	hash *= DeckSize;
	hash += c3;
	hash *= DeckSize;
	hash += c4;
	hash *= DeckSize;
	hash += c5;
	return hash;
}

const auto LargestHandHash = HandHash(47, 48, 49, 50, 51);
const auto HandHashCount = static_cast<size_t>(LargestHandHash) + 1;

class AllHands
{
public:
	AllHands()
	{
		handRanksByHash.resize(HandHashCount);

		for (auto c1 = 0; c1 < DeckSize; ++c1)
			for (auto c2 = c1 + 1; c2 < DeckSize; ++c2)
				for (auto c3 = c2 + 1; c3 < DeckSize; ++c3)
					for (auto c4 = c3 + 1; c4 < DeckSize; ++c4)
						for (auto c5 = c4 + 1; c5 < DeckSize; ++c5)
						{
							auto hash = HandHash(c1, c2, c3, c4, c5);
							handHashByIndex.push_back(hash);

							Hand hand{ c1, c2, c3, c4, c5 };
							handRanksByHash[hash] = hand.GetValue().GetRank();

							++countByHandType[hand.GetValue().GetType()];
						}
	}

	size_t GetDistinctHandCount() const
	{
		return handHashByIndex.size();
	}

	int GetRankByHash(int hash) const
	{
		return handRanksByHash[hash];
	}

	int GetCountByHandType(HandType type) const
	{
		return countByHandType.find(type)->second;
	}

	int GetBestHandRank(int c1, int c2, int m1, int m2, int m3, int m4, int m5) const
	{
		auto rank = GetRank(m1, m2, m3, m4, m5);
		rank = max(rank, GetRank(c1, m1, m2, m3, m4));
		rank = max(rank, GetRank(c1, m1, m2, m3, m5));
		rank = max(rank, GetRank(c1, m1, m2, m4, m5));
		rank = max(rank, GetRank(c1, m1, m3, m4, m5));
		rank = max(rank, GetRank(c1, m2, m3, m4, m5));
		rank = max(rank, GetRank(c2, m1, m2, m3, m4));
		rank = max(rank, GetRank(c2, m1, m2, m3, m5));
		rank = max(rank, GetRank(c2, m1, m2, m4, m5));
		rank = max(rank, GetRank(c2, m1, m3, m4, m5));
		rank = max(rank, GetRank(c2, m2, m3, m4, m5));
		rank = max(rank, GetRank(c1, c2, m1, m2, m3));
		rank = max(rank, GetRank(c1, c2, m1, m2, m4));
		rank = max(rank, GetRank(c1, c2, m1, m2, m5));
		rank = max(rank, GetRank(c1, c2, m1, m3, m4));
		rank = max(rank, GetRank(c1, c2, m1, m3, m5));
		rank = max(rank, GetRank(c1, c2, m1, m4, m5));
		rank = max(rank, GetRank(c1, c2, m2, m3, m4));
		rank = max(rank, GetRank(c1, c2, m2, m3, m5));
		rank = max(rank, GetRank(c1, c2, m2, m4, m5));
		rank = max(rank, GetRank(c1, c2, m3, m4, m5));
		return rank;
	}

	void DumpHandDistribution() const
	{
		cout << GetDistinctHandCount() << " possible hands.\n";
		cout << GetCountByHandType(HandType::RoyalFlush) << " royal flushes\n";
		cout << GetCountByHandType(HandType::StraightFlush) << " straight flushes\n";
		cout << GetCountByHandType(HandType::FourOfAKind) << " four of a kinds\n";
		cout << GetCountByHandType(HandType::FullHouse) << " full houses\n";
		cout << GetCountByHandType(HandType::Flush) << " flushes\n";
		cout << GetCountByHandType(HandType::Straight) << " straights\n";
		cout << GetCountByHandType(HandType::ThreeOfAKind) << " three of a kinds\n";
		cout << GetCountByHandType(HandType::TwoPair) << " two pairs\n";
		cout << GetCountByHandType(HandType::OnePair) << " pairs\n";
		cout << GetCountByHandType(HandType::HighCard) << " high cards\n";
	}

private:
	int GetRank(int c1, int c2, int c3, int c4, int c5) const
	{
		array<int, HandSize> cards{ c1, c2, c3, c4, c5 };
		sort(cards.begin(), cards.end());
		auto hash = HandHash(cards[0], cards[1], cards[2], cards[3], cards[4]);
		return GetRankByHash(hash);
	}

private:
	vector<int> handHashByIndex;
	vector<int> handRanksByHash;
	map<HandType, int> countByHandType;
};

int HoleHash(int c1, int c2)
{
	auto hash = c1;
	hash *= DeckSize;
	hash += c2;
	return hash;
}

class HoleCards
{
public:
	HoleCards() = default;
	HoleCards(const HoleCards& rhs) = default;
	HoleCards(HoleCards&& rhs) = default;
	~HoleCards() = default;

	HoleCards(int hash)
	{
		SetHash(hash);
	}

	HoleCards(const Card& card1, const Card& card2)
	{
		SetCards(card1, card2);
	}

	HoleCards(Card card1, Card card2, const map<Suit, Suit>& suitMap)
	{
		card1.SetSuit(suitMap.find(card1.GetSuit())->second);
		card2.SetSuit(suitMap.find(card2.GetSuit())->second);
		SetCards(card1, card2);
	}

	HoleCards& operator=(const HoleCards& rhs) = default;
	HoleCards& operator=(HoleCards&& rhs) = default;

	void MoveNext()
	{
		++card2;
		if (card2 < DeckSize)
			return;
		++card1;
		card2 = card1 + 1;
	}

	bool IsValid() const
	{
		return card1 >= 0 && card1 < card2 && card2 < DeckSize;
	}

	int GetHash() const
	{
		return card1 * DeckSize + card2;
	}

	void SetHash(int value)
	{
		card1 = value / DeckSize;
		card2 = value % DeckSize;
	}

	void SetCards(const Card& card1, const Card& card2)
	{
		auto id1 = card1.GetId();
		auto id2 = card2.GetId();
		this->card1 = min(id1, id2);
		this->card2 = max(id1, id2);
	}

	int GetCard1() const
	{
		return card1;
	}

	int GetCard2() const
	{
		return card2;
	}

	bool IsDisjoint(const HoleCards& rhs) const
	{
		return !rhs.Contains(card1) && !rhs.Contains(card2);
	}

	bool Contains(int card) const
	{
		return card1 == card || card2 == card;
	}

	bool operator==(const HoleCards& rhs) const
	{
		return card1 == rhs.card1 && card2 == rhs.card2;
	}
	bool operator!=(const HoleCards& rhs) const
	{
		return !operator==(rhs);
	}

	string ToString() const
	{
		return Card{ card1 }.ToString() + " " + Card{ card2 }.ToString();
	}

private:
	int card1 = 0;
	int card2 = 1;
};

class Odds
{
public:
	Odds() = default;
	Odds(const Odds& rhs) = default;
	Odds(Odds&& rhs) = default;
	~Odds() = default;

	Odds(int win, int draw, int lose)
		: win{ win }, draw{ draw }, lose{ lose }
	{
	}

	Odds& operator=(const Odds& rhs) = default;
	Odds& operator=(Odds&& rhs) = default;

	void Compute(const AllHands& allHands, const HoleCards& hole1, const HoleCards& hole2)
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

	int GetWin() const
	{
		return win;
	}
	int GetDraw() const
	{
		return draw;
	}
	int GetLose() const
	{
		return lose;
	}

	Odds Invert() const
	{
		return { lose, draw, win };
	}

	string ToString() const
	{
		ostringstream out;
		out << "Win " << win << ", Draw " << draw << ", Lose " << lose;
		return out.str();
	}

private:
	int win = 0;
	int draw = 0;
	int lose = 0;
};

class OddsCache
{
public:
	OddsCache() = default;
	OddsCache(const OddsCache& rhs) = default;
	OddsCache(OddsCache&& rhs) = default;
	~OddsCache() = default;

	OddsCache& operator=(const OddsCache& rhs) = default;
	OddsCache& operator=(OddsCache&& rhs) = default;

	void AddIfMissing(const AllHands& allHands, const HoleCards& hole1, const HoleCards& hole2)
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

	void PermuteHands(const HoleCards& hole1, const HoleCards& hole2, const Odds& odds)
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

	void PermuteHandsOneSuit(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const set<Suit>& suits)
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

	void PermuteHandsTwoSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const set<Suit>& suits)
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

	void PermuteHandsThreeSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const set<Suit>& suits)
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

	void PermuteHandsFourSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds)
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

	void PermuteSuits(const Card& card1, const Card& card2, const Card& card3, const Card& card4, const Odds& odds, const map<Suit, Suit>& suitMap)
	{
		HoleCards hole1{ card1, card2, suitMap };
		HoleCards hole2{ card3, card4, suitMap };
		auto hash1 = hole1.GetHash();
		auto hash2 = hole2.GetHash();
		cache[{ hash1, hash2 }] = odds;
		cache[{ hash2, hash1 }] = odds.Invert();
	}

	void Load()
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

	void Save() const
	{
		ofstream out{ R"(c:\save\temp\poker.csv)" };
		for (auto& item : cache)
			out << get<0>(item.first) << ','
			<< get<1>(item.first) << ','
			<< item.second.GetWin() << ','
			<< item.second.GetDraw() << ','
			<< item.second.GetLose() << endl;
	}

	void Compute(const AllHands& allHands)
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

private:
	map<tuple<int, int>, Odds> cache;
};

int main()
{
	try
	{
		//cout << "Computing all hand rankings...";
		//AllHands allHands;
		//cout << "done!\n";

		//2598960 possible hands.
		//4 royal flushes
		//36 straight flushes
		//624 four of a kinds
		//3744 full houses
		//5108 flushes
		//10200 straights
		//54912 three of a kinds
		//123552 two pairs
		//1098240 pairs
		//1302540 high cards

		//1326 possible starting hands
		//1225 other starting hands
		//1712304 possible community combinations
		//812175 distinct combinations of starting hands (1326 * 1225 / 2)

		//OddsCache oddsCache;
		//oddsCache.Load();

		//1381800 Permutations for a single handle against two opponents.

		//9 players is considered ideal table size.

		HoleCards hole1{ { Face::Ace, Suit::Spades }, { Face::Ace, Suit::Hearts } };
		auto count = 0;
		for (HoleCards hole2; hole2.IsValid(); hole2.MoveNext())
		{
			if (!hole2.IsDisjoint(hole1))
				continue;
			for (HoleCards hole3; hole3.IsValid(); hole3.MoveNext())
			{
				if (!hole3.IsDisjoint(hole1) || !hole3.IsDisjoint(hole2))
					continue;
				++count;
			}
		}
		cout << count << " Permutations for a single handle against two opponents.\n";
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
