#include "pch.h"
#include "Timer.h"
#include "Deck.h"

vector<pair<int, int>> CreateTwoCards();

void Test6()
{
	cout << "Test6\n";

	//Size: 196
	//Count2: 16569
	//Count3: 800361
	//Count4: 24670043
	//Count5: 513145502
	//Duration: 3313.01ms

	Timer timer;
	auto twoCards = CreateTwoCards();
	LargeInteger count2 = 0, count3 = 0, count4 = 0, count5 = 0;

	struct row_t
	{
		int value;
		vector<int> columns;
	};
	using matrix_t = vector<row_t>;
	map<int, int> rowIndexByValue;
	matrix_t m;
	for (auto& twoCard : twoCards)
	{
		auto iter = rowIndexByValue.find(twoCard.first);
		if (iter == rowIndexByValue.end())
		{
			rowIndexByValue.emplace(twoCard.first, static_cast<int>(m.size()));
			m.push_back({ twoCard.first, { twoCard.second } });
		}
		else
			m[iter->second].columns.push_back(twoCard.second);
	}
	sort(execution::par_unseq, m.begin(), m.end(), [](const row_t& lhs, const row_t& rhs) -> bool
	{
		return (lhs.columns.size() > rhs.columns.size()) ||
			(lhs.columns.size() == rhs.columns.size() && lhs.value < rhs.value);
	});
	using bits_t = bitset<DeckSize>;
	for (auto row1 = 0; row1 < m.size(); ++row1)
	{
		auto& r1 = m[row1];
		for (auto col1 : r1.columns)
		{
			bits_t b1;
			b1.set(r1.value);
			b1.set(col1);
			for (auto row2 = row1 + 1; row2 < m.size(); ++row2)
			{
				auto& r2 = m[row2];
				if (b1.test(r2.value))
					continue;
				for (auto col2 : r2.columns)
				{
					if (b1.test(col2))
						continue;
					auto b2 = b1;
					b2.set(r2.value);
					b2.set(col2);
					++count2;
					for (auto row3 = row2 + 1; row3 < m.size(); ++row3)
					{
						auto& r3 = m[row3];
						if (b2.test(r3.value))
							continue;
						for (auto col3 : r3.columns)
						{
							if (b2.test(col3))
								continue;
							auto b3 = b2;
							b3.set(r3.value);
							b3.set(col3);
							++count3;
							for (auto row4 = row3 + 1; row4 < m.size(); ++row4)
							{
								auto& r4 = m[row4];
								if (b3.test(r4.value))
									continue;
								for (auto col4 : r4.columns)
								{
									if (b3.test(col4))
										continue;
									auto b4 = b3;
									b4.set(r4.value);
									b4.set(col4);
									++count4;
									for (auto row5 = row4 + 1; row5 < m.size(); ++row5)
									{
										auto& r5 = m[row5];
										if (b4.test(r5.value))
											continue;
										for (auto col5 : r5.columns)
										{
											if (b4.test(col5))
												continue;
											++count5;
										}
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
	cout << "Count2: " << count2 << '\n';
	cout << "Count3: " << count3 << '\n';
	cout << "Count4: " << count4 << '\n';
	cout << "Count5: " << count5 << '\n';
	cout << "Duration: " << timer.GetDurationMs() << "ms\n";
	cout << '\n';
}
