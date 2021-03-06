#pragma once

using CombinationSequence = vector<pair<uint32_t, uint32_t>>;

class CombinationSequences
{
public:
	CombinationSequences();
	CombinationSequences(const CombinationSequences& rhs) = default;
	CombinationSequences(CombinationSequences&& rhs) = default;
	~CombinationSequences() = default;

	CombinationSequences& operator=(const CombinationSequences& rhs) = default;
	CombinationSequences& operator=(CombinationSequences&& rhs) = default;

	const CombinationSequence& operator[](uint64_t n) const;

	static const CombinationSequence& Get(uint64_t n);

private:
	vector<CombinationSequence> sequences;
};
