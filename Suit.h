#pragma once

enum class Suit
{
	Spades = 0,
	Hearts,
	Clubs,
	Diamonds
};

const auto SuitCount = 4;

string ToString(Suit value);
