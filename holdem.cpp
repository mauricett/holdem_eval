#include <iostream>
#include <string>
#include <algorithm>
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
	int *cards[52];


	for (int i = 0; i < 52; ++i)
	{
		cards[i] = &cds[i][0];
	}
	//Evtl interessant für Shuffle.
	//Scheint zu funktionieren (sollt ich aber noch testen):
	cards[36] = cards[3];

	/*int board1[10] = { 1, 3, 1, 4, 7, 0, 0, 0, 0, 0 };
	int hands1[24] = { 2, 5, 0, 0, 4, 5, 0, 0, 10, 12, 0, 0, 5, 6, 0, 0 };*/
	//int board1[10] = { 1, 2, 3, 9, 10, 1, 1, 0, 0, 3 };
	//int hands1[24] = { 4, 5, 0, 0, 0, 4, 0, 0, 0, 12, 0, 0, 5, 8, 1, 3 };
	int board1[10] = { 1, 1, 9, 9, 9, 1, 2, 0, 0, 2 };
	int hands1[24] = { 6, 9, 2, 3, 5, 9, 0, 3, 4, 6, 1, 1, 3, 3, 1, 3 };
	find_winner(board1, hands1, merged, 4);

	/*
	include <random>
	random_device rd;   // non-deterministic generator  
	mt19937 gen(rd());  // to seed mersenne twister.  
	uniform_int_distribution<> dist(1, 6); // distribute results between 1 and 6 inclusive.  

	for (int i = 0; i < 5; ++i) {
		cout << dist(gen) << " "; // pass the generator to the distribution.  
	}
	cout << endl;

	set tabstop=4
	set softtabstop=4
	set shiftwidth=4
	set noexpandtab
	set filetype=c++
	set filetype=cpp
	set colorcolumn=110
	set?? highlight ColorColumn ctermbg=darkgray
	syntax on

	*/

	getchar();
	return 0;
}
