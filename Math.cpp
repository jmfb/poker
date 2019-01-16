#include "Math.h"

long long Combinations(long long total, long long take)
{
	long long result = 1;
	for (long long index = 0; index < take; ++index)
		result = result * (total - index) / (index + 1);
	return result;
}

long long Partitions(long long count)
{
	return count == 2 ? 1 : (count - 1) * Partitions(count - 2);
}
