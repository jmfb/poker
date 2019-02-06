#pragma once

enum class Suit
{
	Spades = 0,
	Hearts,
	Clubs,
	Diamonds
};

const auto SuitCount = 4;

const array<Suit, SuitCount> AllSuits{ Suit::Spades, Suit::Hearts, Suit::Clubs, Suit::Diamonds };

string ToString(Suit value);
