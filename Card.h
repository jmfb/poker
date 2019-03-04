#pragma once
#include "Suit.h"
#include "Face.h"

class Card
{
public:
	Card() = default;
	Card(const Card& rhs) = default;
	Card(Card&& rhs) = default;
	Card(int id);
	Card(Suit suit, Face face);
	Card(Face face, Suit suit);
	~Card() = default;

	Card& operator=(const Card& rhs) = default;
	Card& operator=(Card&& rhs) = default;

	void SetId(int id);
	int GetId() const;
	void Set(Suit suit, Face face);
	void SetSuit(Suit value);
	void SetFace(Face value);
	Suit GetSuit() const;
	Face GetFace() const;
	void SwitchSuit(const map<Suit, Suit>& suitMap);

	bool operator==(const Card& rhs) const;
	bool operator!=(const Card& rhs) const;

	string ToString() const;

private:
	Suit suit = Suit::Spades;
	Face face = Face::Ace;
};
