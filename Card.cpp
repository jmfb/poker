#include "pch.h"
#include "Card.h"
#include "Face.h"
#include "Suit.h"
#include "Deck.h"

Card::Card(int id)
{
	SetId(id);
}

Card::Card(Suit suit, Face face)
{
	Set(suit, face);
}

Card::Card(Face face, Suit suit)
{
	Set(suit, face);
}

void Card::SetId(int id)
{
	if (id < 0 || id >= DeckSize)
		throw runtime_error{ "Invalid card id." };
	suit = static_cast<Suit>(id / FaceCount);
	face = static_cast<Face>(id % FaceCount);
}

int Card::GetId() const
{
	return static_cast<int>(suit) * FaceCount + static_cast<int>(face);
}

void Card::Set(Suit suit, Face face)
{
	this->suit = suit;
	this->face = face;
}

void Card::SetSuit(Suit value)
{
	suit = value;
}

void Card::SetFace(Face value)
{
	face = value;
}

Suit Card::GetSuit() const
{
	return suit;
}

Face Card::GetFace() const
{
	return face;
}

void Card::SwitchSuit(Suit original, Suit replacement)
{
	if (suit == original)
		suit = replacement;
}

bool Card::operator==(const Card& rhs) const
{
	return suit == rhs.suit && face == rhs.face;
}

bool Card::operator!=(const Card& rhs) const
{
	return !operator==(rhs);
}

string Card::ToString() const
{
	return ::ToString(face) + ::ToString(suit);
}
