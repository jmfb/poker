#include "pch.h"
#include "Timer.h"
#include "Deck.h"
#include "Counts.h"

vector<pair<int, int>> CreateTwoCards();

//Expected Values
//Per: 15225893865 67445820 246753 666 1
//Count: 196 16569 800361 24670043 513145502
//Lose: 2048339780657

//Actual (correct, but slow)
//Size: 196
//Count2: 16569
//Count3: 800361
//Count4: 24670043
//Count5: 513145502
//Duration: 8245.95ms (6281.57ms)
void Test5()
{
	cout << "Test5\n";

	Timer timer;
	auto twoCards = CreateTwoCards();
	using row_t = bitset<DeckSize>;
	cout << "Size of row: " << (sizeof(row_t) * CHAR_BIT) << " bits\n";
	using matrix_t = array<row_t, DeckSize>;
	cout << "Size of matrix: " << sizeof(matrix_t) << " bytes\n";
	matrix_t m;
	for (auto& twoCard : twoCards)
		m[twoCard.first].set(twoCard.second);
	Counts counts;
	for (auto row1 = 0; row1 < DeckSize; ++row1)
	{
		auto& r1 = m[row1];
		if (r1.none())
			continue;
		for (auto col1 = row1 + 1; col1 < DeckSize; ++col1)
		{
			if (!r1.test(col1))
				continue;
			row_t mask1;
			mask1.flip();
			mask1.set(col1, false);
			for (auto row2 = row1 + 1; row2 < DeckSize; ++row2)
			{
				if (!mask1.test(row2))
					continue;
				auto r2 = m[row2] & mask1;
				if (r2.none())
					continue;
				for (auto col2 = row2 + 1; col2 < DeckSize; ++col2)
				{
					if (!r2.test(col2))
						continue;
					++counts.count2;
					auto mask2 = mask1;
					mask2.set(col2, false);
					for (auto row3 = row2 + 1; row3 < DeckSize; ++row3)
					{
						if (!mask2.test(row3))
							continue;
						auto r3 = m[row3] & mask2;
						if (r3.none())
							continue;
						for (auto col3 = row3 + 1; col3 < DeckSize; ++col3)
						{
							if (!r3.test(col3))
								continue;
							++counts.count3;
							auto mask3 = mask2;
							mask3.set(col3, false);
							for (auto row4 = row3 + 1; row4 < DeckSize; ++row4)
							{
								if (!mask3.test(row4))
									continue;
								auto r4 = m[row4] & mask3;
								if (r4.none())
									continue;
								for (auto col4 = row4 + 1; col4 < DeckSize; ++col4)
								{
									if (!r4.test(col4))
										continue;
									++counts.count4;
									auto mask4 = mask3;
									mask4.set(col4, false);
									for (auto row5 = row4 + 1; row5 < DeckSize; ++row5)
									{
										if (!mask4.test(row5))
											continue;
										auto r5 = m[row5] & mask4;
										counts.count5 += r5.count();
									}
								}
							}
						}
					}
				}
			}
		}
	}

	cout << "Size: " << twoCards.size() << '\n';
	counts.Dump();
	cout << "Duration: " << timer.GetDurationMs() << "ms\n";
	cout << '\n';
}
