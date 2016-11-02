#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include "eval.h"

using namespace std;

//Folgendes Board zur verbesserten Lesbarkeit bei Usereingabe, d.h. brauchen noch translate() Funktion.
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

int merged[cardcnt * player]; //ohne suits

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//MASTER PLAN: 
//"master funktion" in der alle allocations von eval_ranks gemacht werden
//und dann eval_ranks in der funktion callen
//so sparn wir uns die ständigen allocations.
//evtl einen zusammenhängenden speicher allocaten?!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



	//!!!!!!!!!!!!!!!!
	//WICHTIG!!!!!!!!!
	//EXPERIMENT DAS KLAPPEN MUSS: 012345A mit straight flush (kein wheelie sf), normaler sf muss gewinnen!
	//NOCH EINS: HABEN NORMALE STRAIGHT & WHEELIE SF -> WHEELIE SF MUSS GEWINNEN
	//CHECKEN: JEDER PLAYER_RANKS[] WIRD NUR EINMAL GESETZT? wichtig!
	//!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!




int main()
{
	int board[10]{ 0,5, 4, 1, 2, 3, 3, 3 , 0, 3 }; // = { (rank1, rank2 ...), (suit1, suit2 ...) }; // glaube ist besser, weil wir ja sortieren müssen
	int hands[6 * 2 * 2]{ 6, 6, 0, 1 }; // (rank1, rank2, ...), (suit1, suit2 ...), ...

	/*0 = win or tie; 1 = total number of players; 2 = number of winning/tieing players; 3 = winning rank; 4 to 9 = players; 
	z.b. win -> 2 == 1, 4 == winner; 5 to up to 9 == losers
	oder tie -> 2 == number of tying;*/
	int result[1 + 2 * 6] = {}; //result[0] = anzahl der simulierten hände; result[1 + 2*p] = player p's wins; result[1 + 2*p + 1] = player p's ties
	double tie[7] = {}; //count tie equity ... tie/playrs_left ... tie[6] == #ties


	/*int board1[10] = { 1, 3, 1, 4, 7, 0, 0, 0, 0, 0 };
	int hands1[24] = { 2, 5, 0, 0, 4, 5, 0, 0, 10, 12, 0, 0, 5, 6, 0, 0 };*/
	//int board1[10] = { 1, 2, 3, 4, 10, 1, 1, 0, 0, 3 };
	//int hands1[24] = { 6, 12, 2, 0, 3, 4, 0, 0, 0, 12, 1, 1, 6, 12, 1, 3 }; //getting bad results here
	//int hands1[24] = { 2, 3, 0, 1, 7, 8, 2, 3, 10, 11, 1, 1, 5, 5, 0, 1 }; //a little bit better results .. looks like problem with flushs, straights or wheelie
	int hands1[24] = { 2, 3, 0, 1, 7, 8, 2, 3 };
	int board1[10] = { 1, 0, 6, 10, 10, 1, 2, 0, 1, 3 };
	//int board1[10] = { 1, 1, 9, 9, 9, 1, 2, 0, 0, 2 };
	//int hands1[24] = { 6, 9, 2, 3, 5, 9, 0, 3, 4, 6, 1, 1, 3, 3, 1, 3 };
	/*generate_board(board1, hands1, cds, 4);
	find_winner(board1, hands1, merged, result, tie, 4);*/
	
	int draw = 0;
	
	for (int n = 0; n < 1000000; ++n)
	{
		int result2[1 + 2 * 6] = {};
		generate_board(board1, hands1, cds, 2);
		cout << "Board: ";
		for (int c = 0; c < 5; ++c)
		{
			cout << translate(board1[c], board1[c + 5], crds) << ' ';
		}
		cout << endl;
		for (int p = 0; p < 2; ++p)
		{
			cout << "Player " << p << ": " << translate(hands1[4 * p + 1], hands1[4 * p + 3], crds) << ' ' << translate(hands1[4 * p], hands1[4 * p + 2], crds) << endl;
		}
		find_winner(board1, hands1, merged, result2, tie, draw, 2);
		cout << endl << "Win/Tie:" << endl;
		for (int p = 0; p < 2; ++p)
		{
			cout << "player" << p << " win: " << result2[1 + 2 * p] << " and tie: " << tie[p] << endl;
		}
			
		getchar();
	}
	
	//probleme:
	//1)
	//board = 8c 7s 3h 6s 5h 

	
	/*
	for (int n = 1; n < 12; ++n)
	{
		for (int i = 0; i < 1000000; ++i)
		{
			generate_board(board1, hands1, cds, 2);
			find_winner(board1, hands1, merged, result, tie, draw, 2);
		}
		cout << n << " millionen" << endl;
	}
	double equity[2] = {};
	for (int i = 0; i < 2; ++i)
	{
		equity[i] = (double)result[1 + 2 * i] / (double)result[0];
		cout << "equity player " << i << ": " << equity[i] << endl;
	}
	int sum = 0;
	for (int i = 0; i < 6; ++i)
	{
		sum += tie[i] + result[1 + 2 * i];
	}
	cout << "summe: " << sum << endl;
	cout << "result[0] = " << result[0] << endl;
	cout << "tie[6] = " << tie[6] << endl;*/


	getchar();
	return 0;
}
