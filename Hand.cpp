#include "pch.h"
#include "Hand.h"
#include "Deck.h"

Hand::Hand(int c1, int c2, int c3, int c4, int c5)
{
	Set(c1, c2, c3, c4, c5);
}

Hand::Hand(int hash)
{
	SetHash(hash);
}

void Hand::Set(int c1, int c2, int c3, int c4, int c5)
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

HandValue Hand::DetermineValue() const
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
		return get<1>(lhs) > get<1>(rhs) || (get<1>(lhs) == get<1>(rhs) && get<0>(lhs) > get<0>(rhs));
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

const array<Card, HandSize>& Hand::GetCards() const
{
	return cards;
}

const HandValue& Hand::GetValue() const
{
	return value;
}

string Hand::ToString() const
{
	return cards[0].ToString() + " " +
		cards[1].ToString() + " " +
		cards[2].ToString() + " " +
		cards[3].ToString() + " " +
		cards[4].ToString();
}

void Hand::SetHash(int value)
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

int Hand::GetHash(int c1, int c2, int c3, int c4, int c5)
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

bool Hand::operator==(const Hand& rhs) const
{
	return cards == rhs.cards;
}

bool Hand::operator!=(const Hand& rhs) const
{
	return !operator==(rhs);
}

bool Hand::IsFlush() const
{
	auto suit = cards[0].GetSuit();
	return suit == cards[1].GetSuit() &&
		suit == cards[2].GetSuit() &&
		suit == cards[3].GetSuit() &&
		suit == cards[4].GetSuit();
}

optional<Face> Hand::IsStraightTo(const array<Face, HandSize>& highCards) const
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

array<Face, HandSize> Hand::GetHighCards() const
{
	array<Face, HandSize> highCards;
	for (auto index = 0; index < HandSize; ++index)
		highCards[index] = cards[index].GetFace();
	sort(highCards.begin(), highCards.end(), greater<Face>());
	return highCards;
}
