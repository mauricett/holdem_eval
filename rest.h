#pragma once
#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

constexpr int cardcnt = 7;
constexpr int player = 6;

//approach: alle starthände sofort mit board std::merge() vereinigen und dif[i] für die <= 7 card hands für <= 6 player ausrechnen
//wichtig:drei verschiedene eval stufen denkbar, jede hat einen wichtigen vorteil:
// 1) nur den winner evaluaten //wenn es einen klaren winner gibt, kann die berechnung abgebrochen werden. gut für monte carlo equity!
// 2) ranks only //vereinfachte version von 3), falls kicker nicht benötigt werden um ranking zu bestimmen -> spart rechenzeit
// 3) ranks & kicker //um an jedem spot das ranking einschätzen zu können -> awareness über draws, die man mit allin equity nicht hat!
void eval_ranks(int(&hands)[6 * 2 * 2], int(&board)[10], int(&merged)[cardcnt * player], const int player)
{
	//int ranks[6] = {}; //ranks der player: 1-13 X-high; 21-33 X-pair; 41-53 two-pair; 61-73 trips; 81-93 straight; 101-113 flush; 121-133 FH; 141

	sort(begin(board), begin(board) + 5);
	//straights möglich?
	int str_check[4] = {};
	bool straight_possible = 1;
	bool double_cards = 0;
	int zero_gap_counter = 0;

	for (int i = 0; i < 4; ++i)
	{
		str_check[i] = board[i + 1] - board[i];
		if (str_check[i] == 1) { ++zero_gap_counter; }
		if (str_check[i] == 0) { double_cards = 1; }
	}
	//!!!!!!!!!!!!!!!!!!!!!!!!!
	//WICHTIG: easy neue bedingung, die ich noch einbauen muss und allen übergeordnet ist -> die mittlerste dif muss 0 oder 1 sein, bei einer straight, siehe schmierzettel!
	//!!!!!!!!!!!!!!!!!!!!!!!!!
	//eine handvoll bedingungen um straights auszuschließen:
	int* min_dif = min_element(begin(str_check), end(str_check));
	if ((board[0] > 0) && (*min_dif > 2)) { straight_possible = 0; }
	else if ((board[0] == 0) && (str_check[0] > 3) && (*min_dif > 2)) { straight_possible = 0; } //straights für min_dif > 2 ausgeschlossen, abgesehen von wheel-ausnahmen
	if (straight_possible && (board[0] > 2)) //nach mustern in den differenzen str_check suchen, die auf straight-möglichkeit schließen lassen
	{
		straight_possible = 0;
		for (int i = 0; i < 3; ++i)
		{
			if (((str_check[i] == 2) && ((str_check[i + 1] == 2) || (str_check[i + 1] == 1)))) { straight_possible = 1; }
			else if (((str_check[i] == 1) && ((str_check[i + 1] == 2) || (str_check[i + 1] == 3)))) { straight_possible = 1; }
			else if (((str_check[i] == 3) && (str_check[i + 1] == 1))) { straight_possible = 1; }
		}
	}
	if (zero_gap_counter > 1) { straight_possible = 1; }


	for (int i = 0; i < player; ++i)
	{
		merge(hands + 4 * i, hands + 2 + 4 * i, board, board + 5, merged + 7 * i);
	}
	//es lohnt sich die flush karten des boards schonmal zu zählen:
	int flush_cnt[4 + 4] = { 0, 0, 0, 0, 0, 1, 2, 3 }; //vier 0-initializer und (0,1,2,3) sind die suits.
	for (int i = 5; i < 10; ++i)
	{
		++flush_cnt[board[i]];
	}
	//notiz: ich könnt das bestimmen eines max_elements auch direkt in die obere schleife packen. performance unterschiede?

	//flush möglich?
	int* max_elmnt = max_element(flush_cnt, flush_cnt + 4);
	int has_flush[6] = {};
	int flushs = 0;
	int max_suit = *(max_elmnt + 4); //suit des max_elements

	if (*max_elmnt > 2)
	{
		switch (*max_elmnt)
		{
		case 3:
			for (int i = 0; i < player; ++i)
			{
				if ((hands[4 * i + 2] == max_suit) && (hands[4 * i + 3] == max_suit))
				{
					++has_flush[i];
					++flushs;
				}
			}
			break;
		case 4:
			for (int i = 0; i < player; ++i)
			{
				if ((hands[4 * i + 2] == max_suit) || (hands[4 * i + 3] == max_suit))
				{
					++has_flush[i];
					++flushs;
				}
			}
			break;

		case 5:
			for (int i = 0; i < player; ++i)
			{
				++has_flush[i];
				++flushs;
			}
			break;

		default:
			cout << "FEHLER IN FLUSH SEKTION" << endl;
		}
	}

	//pairs,2pairs,...,fh,quads und straight
	int dif[6 * 6] = {}; //maximal 6 differenzen für 6 player
	for (int players = 0; players < player; ++players)
	{
		for (int i = 0; i < 6; ++i)
		{
			dif[i + 6 * players] = merged[7 * players + i + 1] - merged[7 * players + i];
		}
	}

	//falls es keine double cards gibt, müssen wir FH und Quads nicht überprüfen.
	//wenn wir flush oder straight finden (und straight flush ausschließen) steht der gewinner fest
	//innerhalb des if-statements:
	//zwei zweige, einmal für player mit flush, einmal ohne. mit flush muss nur noch FH, Quads, Straight Flush, Royal gecheckt werden.
	//switch für cases 3,4,5 ?!?!?
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//VIELVERSPRECHENDER SPEED BOOST????
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if ((flushs == 0) && (double_cards == 0))
	{
		switch (straight_possible)
		{
		case 0:
			//only need to check 2pair -> pair -> high card
			break;
		case 1:
			//need straight -> 2pair ...
			break;
		}
	}

	else if ((flushs > 0) && (double_cards == 0))
	{
		switch (straight_possible)
		{
		case 0:
			//flushs=1, we got a winner. else, keep checking. also check for straight flush.
			break;
		case 1:
			break;
		}
	}

	else if (straight_possible && (double_cards == 0))
	{

	}

	else
	{

	}

	cout << "Straight possible? " << straight_possible << endl;
}
