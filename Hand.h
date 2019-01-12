#pragma once
#include "HandValue.h"
#include "HandSize.h"
#include "Card.h"
#include <array>
#include <string>
#include <optional>
using namespace std;

class Hand
{
public:
	Hand() = default;
	Hand(const Hand& rhs) = default;
	Hand(Hand&& rhs) = default;
	Hand(int c1, int c2, int c3, int c4, int c5);
	Hand(int hash);
	~Hand() = default;

	Hand& operator=(const Hand& rhs) = default;
	Hand& operator=(Hand&& rhs) = default;

	void Set(int c1, int c2, int c3, int c4, int c5);
	HandValue DetermineValue() const;

	const array<Card, HandSize>& GetCards() const;
	const HandValue& GetValue() const;
	string ToString() const;

	void SetHash(int value);
	static int GetHash(int c1, int c2, int c3, int c4, int c5);

	bool operator==(const Hand& rhs) const;
	bool operator!=(const Hand& rhs) const;

private:
	bool IsFlush() const;
	optional<Face> IsStraightTo(const array<Face, HandSize>& highCards) const;
	array<Face, HandSize> GetHighCards() const;

private:
	array<Card, HandSize> cards = {};
	HandValue value;
};

const auto LargestHandHash = Hand::GetHash(47, 48, 49, 50, 51);
const auto HandHashCount = static_cast<size_t>(LargestHandHash) + 1;
