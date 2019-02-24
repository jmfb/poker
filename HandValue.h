#pragma once
#include "HandType.h"
#include "HandSize.h"
#include "Face.h"

class HandValue
{
public:
	HandValue() = default;
	HandValue(const HandValue& rhs) = default;
	HandValue(HandValue&& rhs) = default;
	HandValue(HandType type, const array<Face, HandSize>& faces);
	HandValue(int rank);
	~HandValue() = default;

	HandValue& operator=(const HandValue& rhs) = default;
	HandValue& operator=(HandValue&& rhs) = default;

	HandType GetType() const;
	array<Face, HandSize> GetFaces() const;

	string ToString() const;

	bool operator==(const HandValue& rhs) const;
	bool operator!=(const HandValue& rhs) const;
	bool operator<(const HandValue& rhs) const;
	bool operator<=(const HandValue& rhs) const;
	bool operator>(const HandValue& rhs) const;
	bool operator>=(const HandValue& rhs) const;

	int GetRank() const;
	void SetRank(int value);

private:
	HandType type = HandType::HighCard;
	array<Face, HandSize> faces = {};
};
