#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <random>
#include "eval.h"
#include <omp.h>

using namespace std;

string crds[52] =
{
	"2s", "2d", "2h", "2c",
	"3s", "3d", "3h", "3c",
	"4s", "4d", "4h", "4c",
	"5s", "5d", "5h", "5c",
	"6s", "6d", "6h", "6c",
	"7s", "7d", "7h", "7c",
	"8s", "8d", "8h", "8c",
	"9s", "9d", "9h", "9c",
	"Ts", "Td", "Th", "Tc",
	"Js", "Jd", "Jh", "Jc",
	"Qs", "Qd", "Qh", "Qc",
	"Ks", "Kd", "Kh", "Kc",
	"As", "Ad", "Ah", "Ac"
};


int cds[52][2] =
{
	{ 0,0 },{ 0,1 },{ 0,2 },{ 0,3 },
	{ 1,0 },{ 1,1 },{ 1,2 },{ 1,3 },
	{ 2,0 },{ 2,1 },{ 2,2 },{ 2,3 },
	{ 3,0 },{ 3,1 },{ 3,2 },{ 3,3 },
	{ 4,0 },{ 4,1 },{ 4,2 },{ 4,3 },
	{ 5,0 },{ 5,1 },{ 5,2 },{ 5,3 },
	{ 6,0 },{ 6,1 },{ 6,2 },{ 6,3 },
	{ 7,0 },{ 7,1 },{ 7,2 },{ 7,3 },
	{ 8,0 },{ 8,1 },{ 8,2 },{ 8,3 },
	{ 9,0 },{ 9,1 },{ 9,2 },{ 9,3 },
	{ 10,0 },{ 10,1 },{ 10,2 },{ 10,3 },
	{ 11,0 },{ 11,1 },{ 11,2 },{ 11,3 },
	{ 12,0 },{ 12,1 },{ 12,2 },{ 12,3 }
};




void equity_demo(int(&hands)[24], int players, int Nmillions)
{
	int merged[cardcnt * player]; //for internal calculations. ignore.
	int board[10]; //assuming pre-flop situation, meaning, no board cards have been dealt yet.
	int result[1 + 2 * 6] = {}; //result[0] = number of hands; result[1 + 2*p] = number of wins player p; result[2 + 2*p] = number of ties;
	double tie[7] = {}; //count tie equity ... tie/playrs_left ... tie[6] == #ties

	//#pragma omp parallel for
	for (int n = 1; n <= Nmillions; ++n)
	{
		for (int i = 0; i < 1000000; ++i)
		{
			generate_board(board, hands, cds, players);
			find_winner(board, hands, merged, result, tie, players);
		}
		cout << n << " millionen" << endl;
	}
	


	double win[6] = {};
	for (int i = 0; i < players; ++i)
	{
		win[i] = (double)result[1 + 2 * i] / (double)result[0];
	}

	cout << "Player" << '\t';
	for (int p = 0; p < players; ++p)
	{
		cout << "#" << p << '\t';
	}
	cout << endl;

	cout << "Hand" << '\t';
	for (int p = 0; p < players; ++p)
	{
		for (int c = 0; c < 2; ++c)
		{
			cout << translate(hands[4 * p + c], hands[4 * p + 2 + c], crds) << " ";
		}
		cout << '\t';
	}
	cout << endl;

	cout << "win%" << '\t';
	for (int p = 0; p < players; ++p)
	{
		cout << fixed << setprecision(2) << (win[p] * 100) << "%" << '\t';
	}
	cout << endl;

	cout << "tie%" << '\t';
	for (int p = 0; p < players; ++p)
	{
		cout << fixed << setprecision(2) << (((double)tie[p] / (double)result[0]) * 100) << "%" << '\t';
	}
	cout << endl;


	cout << "equity%" << '\t';
	for (int p = 0; p < players; ++p)
	{
		cout << fixed << setprecision(2) << ((win[p] + (double)tie[p] / (double)result[0]) * 100) << "%" << '\t';
	}
	cout << endl;


	getchar();
}



int main()
{
	// deuces = 0, treys = 1, fours = 2, ..., jacks = 9, queens = 10, kings = 11, aces = 12;
	// spades = 0, diamonds = 1, hearts = 2, clubs = 4;
	//int board1[10] = { 2, 5, 8, 11, 12, 2, 2, 3, 2, 2 }; //if a board is needed, the format is: { cardvalue1, cardvalue2, ..., cardvalue5, suit1, suit2, ..., suit 5 }

	int hands1[24] = { 11, 12, 2, 0, 4, 5, 0, 1, 0, 0, 1, 3, 9, 9, 1, 2, 12, 12, 3, 1, 5, 5, 0, 2 }; //note: hands[24] is _required_ to be of size 24, even for less than 6 players
	equity_demo(hands1, 6, 3); //6 player hand, do monte carlo simulation with 3 million iterations.


	/*
	//DEBUGGING STUFF
	for (int i = 0; i < 10; ++i)
	{
		for (int n = 0; n < 1000000; ++n)
		{
			int result2[1 + 2 * 6] = {};
			generate_board(board1, hands1, cds, 6);
			find_winner(board1, hands1, merged, result2, tie, draw, 6);
			if (debug == 1)
			{
				cout << "debug: " << debug << endl;
				debug = 0;
				cout << "Board: ";
				for (int c = 0; c < 5; ++c)
				{
					cout << translate(board1[c], board1[c + 5], crds) << ' ';
				}
				cout << endl;
				for (int p = 0; p < 6; ++p)
				{
					cout << "Player " << p << ": " << translate(hands1[4 * p + 1], hands1[4 * p + 3], crds) << ' ' << translate(hands1[4 * p], hands1[4 * p + 2], crds) << endl;
				}
				cout << endl << "Win/Tie:" << endl;
				for (int p = 0; p < 6; ++p)
				{
					cout << "player" << p << " win: " << result2[1 + 2 * p] << " and tie: " << tie[p] << endl;
				}

				getchar();
			}
		}
		cout << i << " millionen" << endl;
	}
	*/
	
	return 0;
}
