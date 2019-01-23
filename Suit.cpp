#include "pch.h"
#include "Suit.h"

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
