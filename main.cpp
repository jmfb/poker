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
	cout << opponents << ": " << index << " of " << count << ": Computing " << hole.ToString() << "...";
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

int main(int argc, char** argv)
{
	try
	{
		if (argc < 2)
		{
			cout << "Enter the number of players.\n";
			return 0;
		}

		auto opponents = stoi(argv[1]);
		if (opponents < 1 || opponents > 8)
		{
			cout << "Please enter a number of opponents between 1 and 8.\n";
			return 0;
		}

		Compute(opponents);
	}
	catch (const exception& exception)
	{
		cout << exception.what() << '\n';
		return -1;
	}
	return 0;
}
