#pragma once

enum class Face
{
	Two = 0,
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
	King,
	Ace
};
const auto FaceCount = 13;

string ToString(Face value);
