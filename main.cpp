#include "pch.h"
#include "AllHands.h"
#include "HoleCards.h"
#include "Card.h"
#include "Suit.h"
#include "Face.h"
#include "LargeOddsComputer.h"

HoleCards CreateHole(int f1, int f2)
{
	if (f1 == f2)
		return { Card{ static_cast<Face>(f1), Suit::Spades }, Card{ static_cast<Face>(f2), Suit::Hearts } };
	if (f1 < f2)
		return { Card{ static_cast<Face>(f1), Suit::Spades }, Card{ static_cast<Face>(f2), Suit::Spades } };
	return { Card{ static_cast<Face>(f2), Suit::Spades }, Card{ static_cast<Face>(f1), Suit::Hearts } };
}

void Compute(ostream& out, const AllHands& allHands, int f1, int f2, int opponents)
{
	auto index = f1 * FaceCount + f2 + 1;
	auto count = FaceCount * FaceCount;
	auto start = std::chrono::system_clock::now();
	auto hole = CreateHole(f1, f2);
	cout << index << " of " << count << ": Computing " << hole.ToString() << "...";
	LargeOddsComputer computer{ allHands, hole, opponents };
	auto odds = computer.GetOdds();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count();
	cout << " Win or draw " << odds.GetWinOrDraw() << ", Lose " << odds.GetLose() << ", Duration " << duration << " seconds\n";
	out << hole.GetCard1() << ','
		<< hole.GetCard2() << ','
		<< hole.ToString() << ','
		<< odds.GetWinOrDraw() << ','
		<< odds.GetLose() << ','
		<< odds.GetTotal() << endl;
}

//About 10hours to compute odds for a given opponent spread
void Compute(int opponents)
{
	AllHands allHands;
	ofstream out{ to_string(opponents) + "-opponent-odds.txt" };
	for (auto f1 = 0; f1 < FaceCount; ++f1)
		for (auto f2 = 0; f2 < FaceCount; ++f2)
			Compute(out, allHands, f1, f2, opponents);
}

struct Data
{
	int card1;
	int card2;
	string name;
	LargeInteger winOrDraw;
	LargeInteger lose;
	LargeInteger total;

	friend istream& operator>>(istream& in, Data& data)
	{
		char comma = ',';
		in >> data.card1 >> comma >> data.card2 >> comma;
		getline(in, data.name, ',');
		in >> data.winOrDraw >> comma >> data.lose >> comma >> data.total;
		return in;
	}

	LargeInteger GetWinPercent() const
	{
		return ((winOrDraw * 1000 / total) + 5) / 10;
	}

	LargeInteger GetScore(LargeInteger max, LargeInteger min) const
	{
		return ((winOrDraw - min) * 100) / (max - min);
	}
};

int main()
{
	try
	{

		ifstream in{ "8-opponent-odds.txt" };
		string line;
		vector<Data> hands;
		while (getline(in, line))
		{
			Data data;
			istringstream{ line } >> data;
			hands.push_back(data);
		}

		sort(hands.begin(), hands.end(), [](auto lhs, auto rhs) { return lhs.winOrDraw > rhs.winOrDraw; });
		auto max = hands.front().winOrDraw;
		auto min = hands.back().winOrDraw;
		for (auto& data : hands)
			cout << data.name << " Win " << data.GetWinPercent() << "% (Score " << data.GetScore(max, min) << ")\n";
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
