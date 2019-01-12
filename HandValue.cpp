#include "HandValue.h"
#include <stdexcept>
using namespace std;

HandValue::HandValue(HandType type, const array<Face, HandSize>& faces)
	: type{ type }, faces{ faces }
{
}

HandValue::HandValue(int rank)
{
	SetRank(rank);
}

HandType HandValue::GetType() const
{
	return type;
}

array<Face, HandSize> HandValue::GetFaces() const
{
	return faces;
}

string HandValue::ToString() const
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

bool HandValue::operator==(const HandValue& rhs) const
{
	return type == rhs.type && faces == rhs.faces;
}

bool HandValue::operator!=(const HandValue& rhs) const
{
	return !operator==(rhs);
}

bool HandValue::operator<(const HandValue& rhs) const
{
	return static_cast<int>(type) < static_cast<int>(rhs.type) || (type == rhs.type && faces < rhs.faces);
}

bool HandValue::operator<=(const HandValue& rhs) const
{
	return !(rhs < *this);
}

bool HandValue::operator>(const HandValue& rhs) const
{
	return rhs < *this;
}

bool HandValue::operator>=(const HandValue& rhs) const
{
	return !operator<(rhs);
}

int HandValue::GetRank() const
{
	auto rank = static_cast<int>(type);
	for (auto face : faces)
	{
		rank *= FaceCount;
		rank += static_cast<int>(face);
	}
	return rank;
}

void HandValue::SetRank(int value)
{
	auto rank = value;
	for (auto& face : faces)
	{
		face = static_cast<Face>(rank % FaceCount);
		rank /= FaceCount;
	}
	reverse(faces.begin(), faces.end());
	type = static_cast<HandType>(rank);
}
