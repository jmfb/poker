#include "pch.h"
#include "Face.h"

string ToString(Face value)
{
	switch (value)
	{
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
	case Face::Ace: return "A";
	}
	throw new runtime_error{ "Invalid face." };
}
