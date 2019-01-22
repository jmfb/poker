#include "Math.h"

LargeInteger Combinations(LargeInteger total, LargeInteger take)
{
	LargeInteger result{ 1 };
	for (LargeInteger index{ 0 }; index < take; ++index)
		result = result * (total - index) / (index + 1);
	return result;
}

LargeInteger Partitions(LargeInteger count)
{
	return count == 2 ? 1 : (count - 1) * Partitions(count - 2);
}
