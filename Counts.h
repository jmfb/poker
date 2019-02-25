#pragma once

//2-card counts are bounded by max 2-card count n=268, opponents k=8, max = nCk.
class Counts
{
public:
	Counts() = default;
	Counts(const Counts& rhs) = default;
	Counts(Counts&& rhs) = default;
	~Counts() = default;

	Counts& operator=(const Counts& rhs) = default;
	Counts& operator=(Counts&& rhs) = default;

	Counts& operator+=(const Counts& rhs);
	Counts operator+(const Counts& rhs) const;

	void Dump() const;

	static Counts GetTotal(const vector<Counts>& counts);

public:
	//Max u32 = 4,294,967,295
	uint32_t count2 = 0; //Max 35,778
	uint32_t count3 = 0; //Max 3,172,316
	uint32_t count4 = 0; //Max 210,165,935
	//Max u64 = 18,446,744,073,709,551,615
	uint64_t count5 = 0; //Max 11,096,761,368
	uint64_t count6 = 0; //Max 486,408,039,964
	uint64_t count7 = 0; //Max 18,205,558,067,224
	uint64_t count8 = 0; //Max 593,956,331,943,183
};
