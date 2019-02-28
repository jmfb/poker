#include "pch.h"
#include "CombinationSequences.h"

CombinationSequences::CombinationSequences()
{
	const auto maxN = 268;
	sequences.resize(maxN + 1);
	for (auto n = 2; n <= maxN; ++n)
	{
		auto size = (n * (n - 1)) / 2;
		auto& sequence = sequences[n];
		for (auto i2 = 1; i2 < n; ++i2)
			sequence.emplace_back(0, i2);
		for (auto p : sequences[n - 1])
			sequence.emplace_back(p.first + 1, p.second + 1);
	}
	//Duration: 36ms
}

const CombinationSequence& CombinationSequences::operator[](uint64_t n) const
{
	return sequences[n];
}

const CombinationSequence& CombinationSequences::Get(uint64_t n)
{
	static CombinationSequences sequences;
	return sequences[n];
}
