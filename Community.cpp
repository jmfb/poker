#include "pch.h"
#include "Community.h"

Community::Community(const BurnAndTurn& flop, const BurnAndTurn& turn, const BurnAndTurn& river)
	: Community{ flop[0], flop[1], flop[2], turn[0], river[0] }
{
}

Community::Community(const BurnAndTurn& flop, const BurnAndTurn& turn, int river)
	: Community{ flop[0], flop[1], flop[2], turn[0], river }
{
}

Community::Community(const BurnAndTurn& flop, int turn, int river)
	: Community{ flop[0], flop[1], flop[2], turn, river }
{
}

Community::Community(int c1, int c2, int c3, int c4, int c5)
	: cards{ c1, c2, c3, c4, c5 }
{
}

int Community::operator[](int index) const
{
	if (index < 0 || index >= cards.size())
		throw runtime_error{ "Invalid index." };
	return cards[index];
}

array<int, 5>::const_iterator Community::begin() const
{
	return cards.begin();
}

array<int, 5>::const_iterator Community::end() const
{
	return cards.end();
}
