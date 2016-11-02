#pragma once
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;
constexpr int cardcnt = 7;
constexpr int player = 6;

random_device rd;
mt19937 gen(rd());

string translate(int high, int suit, string cards[52])
{
	return cards[4 * high + suit];
}
string cards2[52] =
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

//one ugly ass long 900 line function.
//looks for winner among 6 players (with 1 hand each, i.e. not a range of hands!)
void find_winner(int(&board)[10], int(&hands)[6 * 2 * 2], int(&merged)[cardcnt * player], int(&result)[1 + 2 * 6], double(&tie)[7], int draw, const int player)
{
	sort(begin(board), begin(board) + 5);
	for (int i = 0; i < player; ++i)
	{
		merge(hands + 4 * i, hands + 2 + 4 * i, board, board + 5, merged + 7 * i);
	}
	int flush_cnt[4 + 4] = { 0, 0, 0, 0, 0, 1, 2, 3 }; //vier 0-initializer und (0,1,2,3) sind die suits.
	for (int i = 5; i < 10; ++i)
	{
		++flush_cnt[board[i]];
	}
	//all ranks a player achieved, 11 different ranks, max 6 players:
	int player_ranks[6 * 11] = {}; //0 high card, 1 pair, 2 twopair, 3 trips, 4, wheelie, 5 straight, 6 flush, 7 fh, 8 quads, 9 wheelie sf, 10 sf
	int rank_cnt[11] = {};

	//flush möglich?
	int* max_elmnt = max_element(flush_cnt, flush_cnt + 4);
	int has_flush[6] = {};
	int flushs = 0;
	int max_suit = *(max_elmnt + 4); //suit des max_elements						 
	int sflush[7 * 6] = {}; //array with all cards of winning suit, max 7 cards * 6 players
	int sf_dif[6 * 6]; //6 difs * 6 players
	int sf_cnt[6] = {};
	for (int i = 0; i < 42; ++i)
	{
		sflush[i] = -1;
	}
	int flush_cards[6] = {}; //number of flush cards per player

	if (*max_elmnt > 2)
	{
		switch (*max_elmnt)
		{
		case 3:
			for (int i = 0; i < player; ++i)
			{
				if ((hands[4 * i + 2] == max_suit) && (hands[4 * i + 3] == max_suit))
				{
					player_ranks[11 * i + 6] = 1;
					++rank_cnt[6];
					sflush[7 * i] = hands[4 * i];
					sflush[7 * i + 1] = hands[4 * i + 1];
					flush_cards[i] = 2;
				}
			}
			break;
		case 4:
			for (int i = 0; i < player; ++i)
			{
				if ((hands[4 * i + 2] == max_suit) || (hands[4 * i + 3] == max_suit))
				{
					player_ranks[11 * i + 6] = 1;
					++rank_cnt[6];
					if (hands[4 * i + 2] == max_suit) { sflush[7 * i] = hands[4 * i]; ++flush_cards[i]; }
					if (hands[4 * i + 3] == max_suit) { sflush[7 * i + 1] = hands[4 * i + 1]; ++flush_cards[i]; }
				}
			}
			break;

		case 5:
			for (int i = 0; i < player; ++i)
			{
				player_ranks[11 * i + 6] = 1;
				++rank_cnt[6];
				if (hands[4 * i + 2] == max_suit) { sflush[7 * i] = hands[4 * i]; ++flush_cards[i]; }
				if (hands[4 * i + 3] == max_suit) { sflush[7 * i + 1] = hands[4 * i + 1]; ++flush_cards[i]; }
			}
			break;

		default:
			cout << "FEHLER IN FLUSH SEKTION" << endl;
		}
		for (int p = 0; p < player; ++p)
		{
			for (int c = 0; c < 5; ++c)
			{
				if (board[c + 5] == max_suit) { sflush[7 * p + 2 + c] = board[c]; ++flush_cards[p]; }
			}
			sort(begin(sflush) + 7 * p, begin(sflush) + 7 * p + 7);
		}
	}

	//mit cnt_zero_grps und max_size_grp lassen sich alle pair ranks finden
	int cnt_zero_grps[6] = {}; //wieviele gruppen von nullen, d.h. nullen getrennt durch >0 ?
	int max_size_grp[6] = {}; //wie groß ist größte gruppe?
	int dif[6 * 6] = {};
	int straight[6] = {};
	int str_tmp[6] = {};
	int max_tmp[6] = {}; //tmp zum zwischenspeichern und vergleichen der max-größe
	int *sflush_pointer[7 * 6]; //max 7 cards * 6 players
	int *straight_pointer[6];
	int sflushs[6] = {}; //sflushs[player] >= 5 means straight flush

	for (int p = 0; p < player; ++p)
	{
		dif[6 * p] = merged[1 + 7 * p] - merged[7 * p];

		if (dif[6 * p] > 1) {}
		else if (dif[6 * p] == 1) { ++straight[p]; }
		else if (dif[6 * p] == 0) { ++cnt_zero_grps[p]; ++max_size_grp[p]; }
		str_tmp[p] = straight[p];
		max_tmp[p] = max_size_grp[p];

		for (int i = 1; i < (cardcnt - 1); ++i)
		{
			dif[i + 6 * p] = merged[i + 1 + 7 * p] - merged[i + 7 * p];
		}
		for (int i = 1; i < (cardcnt - 1); ++i)
		{
			switch (dif[i - 1 + 6 * p])
			{
			case 1:
			{
				switch (dif[i + 6 * p])
				{
				case 0:
					++cnt_zero_grps[p];
					++max_tmp[p];
					if (max_tmp[p] > max_size_grp[p]) { max_size_grp[p] = max_tmp[p]; }
					break;
				case 1:
					++str_tmp[p];
					if (str_tmp[p] > straight[p]) { straight[p] = str_tmp[p]; }
					if (straight[p] >= 4) {
						straight_pointer[p] = &(merged[7 * p + i + 1]); } 	//pointer auf letzte straight-karte!
					break;
				default:
					max_tmp[p] = 0;
					str_tmp[p] = 0;
					break;
				}
			}
			break;

			case 0:
			{
				switch (dif[i + 6 * p])
				{
				case 0:
					++max_tmp[p];
					if (max_tmp[p] > max_size_grp[p]) { max_size_grp[p] = max_tmp[p]; }
					break;
				case 1:
					max_tmp[p] = 0;
					++str_tmp[p];
					if (str_tmp[p] > straight[p]) { straight[p] = str_tmp[p]; }
					if (straight[p] >= 4) { straight_pointer[p] = &(merged[7 * p + i + 1]); } 	//pointer auf letzte straight-karte!
					break;
				default:
					max_tmp[p] = 0;
					str_tmp[p] = 0;
					break;
				}
			}
			break;

			default:
			{
				switch (dif[i + 6 * p])
				{
				case 0:
					++cnt_zero_grps[p];
					++max_tmp[p];
					if (max_tmp[p] > max_size_grp[p]) { max_size_grp[p] = max_tmp[p]; }
					break;
				case 1:
					str_tmp[p] = 1;
					max_tmp[p] = 0;
					break;
				default:
					max_tmp[p] = 0;
					break;
				}
			}
			}
		}
			
		bool wheelie[6] = {};
		//geschickte bedingung um wheelie möglichkeiten vorzuselektieren:
		if ((merged[7 * p] == 0) && (merged[6 + 7 * p] == 12) && ((dif[6 * p] == 0) || (dif[6 * p] == 1)) && ((dif[6 * p + 1] == 0) || (dif[6 * p + 1] == 1)) && ((dif[6 * p + 2] == 0) || (dif[6 * p + 2] == 1)))
		{
			//wheelie suchen. dass 2 und A in der hand sind, steht schon fest, müssen 3, 4, 5 überprüfen. ... d.h. in unserer nummerierung 1, 2, 3!
			int rest[3] = {}; //buckets für 1, 2 und 3. alle TRUE?
			for (int i = 1; i < (7 - 1); ++i)
			{
				switch (merged[i + 7 * p])
				{
				case 1:
					++rest[0];
					break;
				case 2:
					++rest[1];
					break;
				case 3:
					++rest[2];
					break;
				}
			}
			if (rest[0] && rest[1] && rest[2]) { wheelie[p] = 1; player_ranks[11 * p + 4] = 1; ++rank_cnt[4]; }
		}

		//cout << endl << endl << "Player " << p + 1 << ":" << endl;
		//check for wheelie, straight, flush, wheelie straight flush, straight flush
		//WICHTIG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//MÜSSEN BEACHTEN: MAN KANN NORMALE STRAIGHT HABEN UND WHEELIE STRAIGHT FLUSH!!!!!!!!!!!!!
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	

		if (straight[p] >= 4)//check for straight and straight flush
		{
			int cntr = 0;
			switch (player_ranks[p * 11 + 6])
			{
			case 0:
				player_ranks[p * 11 + 5] = 1; ++rank_cnt[5];
				break;
			case 1:
				//check for straightflush
				//add board to int sflush[7*6];								
				for (int i = 0; i < (cardcnt - 1); ++i)
				{
					if (sflush[7 * p + i] >= 0) { sf_dif[i + 6 * p] = sflush[i + 1 + 7 * p] - sflush[i + 7 * p]; ++cntr; }
				}
				//(flush_cards-1) is the number of difs between our flush cards, 6-% is the actual starting position,
				//considering we dont always have 7 flush cards (so the first few entries will be empty and be equal to -1,
				//due to my initialization of sf_dif; ... this is meant to differentiate between card rank "2" == 0 and "not a card".
				if (sf_dif[6 * p + (6 - (flush_cards[p] - 1))] == 1) { sflush_pointer[7 * p] = &(sflush[7 * p + (6 - (flush_cards[p] - 1))]); ++sflushs[p]; ++sf_cnt[p]; }
				for (int i = 1; i < cntr; ++i) //cntr ist die anzahl der difs.
				{
					switch (sf_dif[i - 1 + 6 * p + (6 - (flush_cards[p] - 1))])
					{
					case 1:
					{
						switch (sf_dif[i + 6 * p + (6 - (flush_cards[p] - 1))])
						{
						case 1:
							sflush_pointer[7 * p + sflushs[p]] = &(sflush[7 * p + i + (6 - (flush_cards[p] - 1))]);
							++sflushs[p];
							if (sflushs[p] > sf_cnt[p]) { sf_cnt[p] = sflushs[p]; }
							break;
						default:
							sflushs[p] = 0;
							break;
						}
					}
					break;

					default:
					{
						switch (dif[i + 6 * p + (6 - (flush_cards[p] - 1))])
						{
						case 1://PROBLEM .... difs = {1, 1, 1, 1, !=1, 1} setzt einen pointer auf die letzte 1 ... daher if(i<4), hoffe das reicht
							if (i < 4) { sflush_pointer[7 * p + sflushs[p]] = &(sflush[7 * p + i + (6 - (flush_cards[p] - 1))]); }
							++sflushs[p];
							break;
						default:
							sflushs[p] = 0;
							break;
						}
					}
					}
				}
				if (sf_cnt[p] > 3) { player_ranks[11 * p + 10] = 1; ++rank_cnt[10]; }
				//else { player_ranks[11 * p + 5] = 1; ++rank_cnt[5]; } //unnötig, haben hier sowieso mindestens flush.
				break;
			default:
				cout << "fehler in straight flush sektion" << endl;
				break;
			}
		}
		if ((wheelie[p] == 1) && (player_ranks[11 * p + 6] == 1) && (player_ranks[11 * p + 10] == 0)) //if no sf -> check wheelie sf
		{ /*check for wheelie sf*/
			if ((sflush[7 - flush_cards[p] + 7 * p] == 0) && (sflush[6 + 7 * p] == 12) && (sflush[7 - flush_cards[p] + 1 + 7 * p] == 1) && (sflush[7 - flush_cards[p] + 2 + 7 * p] == 2) && (sflush[7 - flush_cards[p] + 3 + 7 * p] == 3))//&& (sf_dif[6 * p + (6 - (flush_cards[p] - 1))] == 1) && (sf_dif[6 * p + (6 - (flush_cards[p] - 1))] == 1) && (sf_dif[6 * p + (6 - (flush_cards[p] - 1))] == 1)
			{
				player_ranks[11 * p + 9] = 1; ++rank_cnt[9];
			}
		}
		//else if (wheelie[p] == 1) { cout << "wheelie!" << endl; } //müssen rank counts hier nicht updaten. haben wir schon gemacht für wheelie!

		//dient nur zum debuggen und kann ganz weg. ich lass es mal als erinnerung drin, weil es instruktiv ist, mit dem sf_cnt + 1;
		/*if (player_ranks[11 * p + 10] == 1) {
			cout << "straight flush cards player " << p << ':' << endl;
			for (int c = 0; c < sf_cnt[p] + 1; c++)
			{
				cout << *(sflush_pointer[7 * p] + c) << ' ';
			}
			cout << endl;
		}*/

		switch (max_size_grp[p])
		{
		case 0:
			++rank_cnt[0];
			player_ranks[p * 11] = 1;
			break;
		case 1:
			if (cnt_zero_grps[p] == 1) { ++rank_cnt[1]; player_ranks[p * 11 + 1] = 1; }
			else { ++rank_cnt[2]; player_ranks[p * 11 + 2] = 1; }
			break;
		case 2:
			if (cnt_zero_grps[p] == 1) { ++rank_cnt[3]; player_ranks[p * 11 + 3] = 1; }
			else { ++rank_cnt[7]; player_ranks[p * 11 + 7] = 1; }
			break;
		case 3:
			++rank_cnt[8]; player_ranks[11 * p + 8] = 1;
			break;
		default:
			cout << "max_size_grp = " << max_size_grp[p] << endl << "error in pair section" << endl;
			for (int i = 0; i < 5; ++i)
			{
				cout << translate(board[i], board[i + 5], cards2);
			}
			getchar();
			break;
		}
	}

	int rank = 10;
	int winning_rank = 0;

	//find winning rank (pair, twopair, etc... 11 different ranks!)
	while (rank)
	{
		if (rank_cnt[rank] != 0) { winning_rank = rank; rank = 0; }
		else { --rank; }
	}
	//more than 1 player with winning rank? tie?
	int involved_players[6]; //maximal 6
	int still_involved[6]; //temp variable um involved_players upzudaten
	int *pointer_to_pair[6];
	int *pointer_two_pair[6]; //points to second highest pair
	int max_pair = 0;
	//int *compare_cards = new int[rank_cnt[winning_rank]];
	int compare_cards[6] = {};
	int playrs_left = rank_cnt[winning_rank];

	if (rank_cnt[winning_rank] == 1) //only one player with winning rank
	{
		for (int p = 0; p < player; ++p)
		{
			if (player_ranks[p * 11 + winning_rank] == 1) { ++result[0]; ++result[1 + 2 * p]; break; }
		}
	}
	else //more than one player with winning rank
	{
		int counter = 0;
		int counter2 = 0;
		for (int p = 0; p < player; ++p)
		{
			if (player_ranks[11 * p + winning_rank] == 1) { involved_players[counter] = p; ++counter; } //z.B. drei spieler 1, 3 und 5 involviert -> involved_players[0] == 1; involved_players[1] == 3; usw
		}
		switch (winning_rank)
		{
		case 0: /*high card*/
			for (int c = 0; c < (cardcnt - 2); ++c) //müssen nur die 5 höchsten karten vergleichen
			{
				counter = 0;
				for (int i = 0; i < playrs_left; ++i)  //for schleife zählt durch involved_players[i];
				{
					compare_cards[i] = merged[7 * involved_players[i] + (cardcnt - 1) - c];
				}
				int max_value = 0;//let's compare the highest high cards in merged
				for (int i = 0; i < playrs_left; ++i)
				{
					if (compare_cards[i] > max_value) { max_value = compare_cards[i]; }
				}
				for (int i = 0; i < playrs_left; ++i)
				{
					if (compare_cards[i] == max_value) { still_involved[counter] = involved_players[i]; ++counter; }
				}
				for (int i = 0; i < counter; ++i)
				{
					involved_players[i] = still_involved[i];
				}

				playrs_left = counter;
				if ((c == (cardcnt - 3)) && (playrs_left > 1))
				{
					/*tie*/
					++result[0];
					++tie[6];
					for (int i = 0; i < playrs_left; ++i)
					{
						++result[2 + 2 * involved_players[i]];
						tie[involved_players[i]] += 1. / playrs_left;
					}
				}
				if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * involved_players[0]]; break; }
			}
			break;
		case 1:/*pair*/
			   //find players who share winning pair, and update involved_players[] accordingly
			counter = 0;
			for (int p = 0; p < playrs_left; ++p)
			{
				for (int c = 5; c >= 0; --c)
				{
					if (dif[c + 6 * involved_players[p]] == 0)
					{
						pointer_to_pair[p] = &(merged[7 * involved_players[p] + c]); //pointer auf die erste karte des paars
						break;
					}
				}
				if (*pointer_to_pair[p] > max_pair) { max_pair = *pointer_to_pair[p]; }
			}
			for (int p = 0; p < playrs_left; ++p)
			{
				if (*pointer_to_pair[p] == max_pair) { still_involved[counter] = involved_players[p]; ++counter; }
			}
			for (int p = 0; p < counter; ++p)
			{
				involved_players[p] = still_involved[p];
			}
			playrs_left = counter;

			//only one winner?
			if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * involved_players[0]];  break; }
			else
			{
				for (int p = 0; p < playrs_left; ++p)
				{
					for (int c = 5; c >= 0; --c)
					{
						if (dif[c + 6 * involved_players[p]] == 0)
						{
							int tmp = 0;
							pointer_to_pair[p] = &(merged[7 * involved_players[p] + c]); //pointer auf die erste karte des paars
							tmp = merged[7 * involved_players[p] + 6]; //letzte element in der 7 card merged hand
							merged[7 * involved_players[p] + 6] = *(pointer_to_pair[p] + 1);
							*(pointer_to_pair[p] + 1) = tmp;
							tmp = merged[7 * involved_players[p] + 5];
							merged[7 * involved_players[p] + 5] = *pointer_to_pair[p];
							*pointer_to_pair[p] = tmp; //vorsicht! ist ab hier nicht mehr pointer aufs pair :D
							break;
						}
					}
					sort(begin(merged) + 7 * involved_players[p], begin(merged) + 7 * involved_players[p] + 5);
				}


				for (int c = 0; c < 3; ++c) //only need to compare the 3 highest cards that aren't pairs
				{
					counter = 0;
					for (int i = 0; i < playrs_left; ++i)  //for schleife zählt durch involved_players[i];
					{
						compare_cards[i] = merged[7 * involved_players[i] + (cardcnt - 3) - c];
					}
					int max_value = 0;//let's compare the highest high cards in merged
					for (int i = 0; i < playrs_left; ++i)
					{
						if (compare_cards[i] > max_value) { max_value = compare_cards[i]; }
					}
					for (int i = 0; i < playrs_left; ++i)
					{
						if (compare_cards[i] == max_value) { still_involved[counter] = involved_players[i]; ++counter; }
					}
					for (int i = 0; i < counter; ++i)
					{
						involved_players[i] = still_involved[i];
					}

					playrs_left = counter;
					if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * involved_players[0]]; break; }
					if ((c == 2) && (playrs_left > 1)) // c==2 means we're in the last loop of for, meaning looking at the lowest of the 5 cards
					{
						/*tie*/
						++result[0];
						++tie[6];
						for (int i = 0; i < playrs_left; ++i)
						{
							++result[2 + 2 * involved_players[i]];
							tie[involved_players[i]] += 1. / playrs_left;
						}
					}
				}
			}
			break;
		case 2:/*twopair*/
			   //find first pair and filter out losers
			counter = 0;
			max_pair = 0;
			for (int p = 0; p < playrs_left; ++p)
			{
				for (int c = 5; c >= 0; --c)
				{
					if (dif[c + 6 * involved_players[p]] == 0)
					{
						pointer_to_pair[p] = &(merged[7 * involved_players[p] + c]); //pointer auf die erste karte des paars
						break;
					}
				}
				if (*pointer_to_pair[p] > max_pair) { max_pair = *pointer_to_pair[p]; }
			}
			for (int p = 0; p < playrs_left; ++p)
			{
				if (*pointer_to_pair[p] == max_pair) { still_involved[counter] = involved_players[p]; ++counter; }
			}
			for (int p = 0; p < counter; ++p)
			{
				involved_players[p] = still_involved[p];
			}
			playrs_left = counter;

			if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * involved_players[0]]; break; }
			//else, zweite pairs vergleichen
			max_pair = 0;
			for (int p = 0; p < playrs_left; ++p)
			{
				counter2 = 0;
				for (int c = 5; c >= 0; --c)
				{
					if ((dif[c + 6 * involved_players[p]] == 0) && (counter2 == 0)) { ++counter2; }
					else if ((dif[c + 6 * involved_players[p]] == 0) && (counter2 == 1))
					{
						pointer_two_pair[p] = &(merged[7 * involved_players[p] + c]); //pointer auf die erste karte des zweiten paars
						break;
					}
				}
				if (*pointer_two_pair[p] > max_pair) { max_pair = *pointer_two_pair[p]; }
			}
			counter = 0;
			for (int p = 0; p < playrs_left; ++p)
			{
				if (*pointer_two_pair[p] == max_pair) { still_involved[counter] = involved_players[p]; ++counter; }
			}
			playrs_left = counter;
			if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * still_involved[0]]; break; }
			//else: überschreiben die beiden twopairs mit nullen und sortieren die restliche liste. dann max elements vergleichen.
			for (int p = 0; p < playrs_left; ++p)
			{
				counter2 = 0;
				for (int c = 5; c >= 0; --c) //pointer neu setzen
				{
					if ((dif[c + 6 * still_involved[p]] == 0) && (counter2 == 1))
					{
						pointer_two_pair[p] = &(merged[7 * still_involved[p] + c]); //pointer auf die erste karte des zweiten paars
						break;
					}
					if ((dif[c + 6 * still_involved[p]] == 0) && (counter2 == 0)) { ++counter2; pointer_to_pair[p] = &(merged[7 * still_involved[p] + c]); }
				}

				*pointer_to_pair[p] = 0;
				*(pointer_to_pair[p] + 1) = 0;
				*pointer_two_pair[p] = 0;
				*(pointer_two_pair[p] + 1) = 0;

				sort(begin(merged) + 7 * still_involved[p], begin(merged) + 7 * still_involved[p] + 7);

				//cout << "high card von player " << still_involved[p] << " ist " << merged[7 * still_involved[p] + 6] << endl;
			}
			counter = 0; //tmp für höchsten kicker
			counter2 = 0; //anzahl spieler mit höchstem kicker
			for (int p = 0; p < playrs_left; ++p)
			{
				if (merged[7 * still_involved[p] + 6] > counter) { counter = merged[7 * still_involved[p] + 6]; }
			}
			for (int p = 0; p < playrs_left; ++p)
			{
				if (merged[7 * still_involved[p] + 6] == counter) { involved_players[counter2] = still_involved[p]; ++counter2; }
			}
			playrs_left = counter2;
			if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * involved_players[0]]; break; }
			else
			{
				/*tie*/
				++result[0];
				++tie[6];
				for (int i = 0; i < playrs_left; ++i)
				{
					++result[2 + 2 * involved_players[i]];
					tie[involved_players[i]] += 1. / playrs_left;

				}
			}
			break;
		case 3:/*trips*/
			counter = 0;
			max_pair = 0;
			for (int p = 0; p < playrs_left; ++p)
			{
				for (int c = 5; c >= 0; --c)
				{
					if (dif[c + 6 * involved_players[p]] == 0)
					{
						pointer_to_pair[p] = &(merged[7 * involved_players[p] + c]); //pointer to middle trips' card, lol.
						break;
					}
				}
				if (*pointer_to_pair[p] > max_pair) { max_pair = *pointer_to_pair[p]; }
			}
			for (int p = 0; p < playrs_left; ++p)
			{
				if (*pointer_to_pair[p] == max_pair) { still_involved[counter] = involved_players[p]; ++counter; }
			}
			playrs_left = counter;
			if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * still_involved[0]]; break; }
			//else: update involved players for further investigation =)
			for (int p = 0; p < playrs_left; ++p)
			{
				involved_players[p] = still_involved[p];
			}
			//taktik: set the trips cards to 0 and sort merged[], then compare highest two cards
			for (int p = 0; p < playrs_left; ++p)
			{
				for (int c = 5; c >= 0; --c)
				{
					if (dif[c + 6 * involved_players[p]] == 0)
					{
						pointer_to_pair[p] = &(merged[7 * involved_players[p] + c]); //pointer to middle trips' card, lol.
						break;
					}
				}

				*pointer_to_pair[p] = 0;
				*(pointer_to_pair[p] + 1) = 0;
				*(pointer_to_pair[p] - 1) = 0;

				sort(begin(merged) + 7 * still_involved[p], begin(merged) + 7 * still_involved[p] + 7);
			}
			counter = 0; //tmp für höchsten kicker
			counter2 = 0; //anzahl spieler mit höchstem kicker
			for (int p = 0; p < playrs_left; ++p)
			{
				if (merged[7 * involved_players[p] + 6] > counter) { counter = merged[7 * involved_players[p] + 6]; }
			}
			for (int p = 0; p < playrs_left; ++p)
			{
				if (merged[7 * involved_players[p] + 6] == counter) { still_involved[counter2] = involved_players[p]; ++counter2; }
			}
			playrs_left = counter2;
			if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * still_involved[0]]; break; }
			//else check second kicker
			for (int p = 0; p < playrs_left; ++p)
			{
				involved_players[p] = still_involved[p];
			}
			counter = 0;
			counter2 = 0;
			for (int p = 0; p < playrs_left; ++p)
			{
				if (merged[7 * involved_players[p] + 5] > counter) { counter = merged[7 * involved_players[p] + 5]; }
			}
			for (int p = 0; p < playrs_left; ++p)
			{
				if (merged[7 * involved_players[p] + 5] == counter) { still_involved[counter2] = involved_players[p]; ++counter2; }
			}
			if (counter2 == 1) { ++result[0]; ++result[1 + 2 * still_involved[0]]; }
			else
			{
				/*tie*/
				++result[0];
				++tie[6];
				for (int i = 0; i < counter2; ++i)
				{
					++result[2 + 2 * still_involved[i]];
					tie[still_involved[i]] += 1. / counter2;

				}
			}

			break;
		case 4:/*wheelie*/
			   //only tie possible!
			++result[0];
			++tie[6];
			for (int i = 0; i < playrs_left; ++i)
			{
				++result[2 + 2 * involved_players[i]];
				tie[involved_players[i]] += 1. / playrs_left;
			}
			break;
		case 5:/*straight*/
			   //straight_pointer[7p] ist pointer auf höchste straight karte
			counter = 0; //use as tmp max
			counter2 = 0; //how many players involved?
						  //only need to check highest card
			for (int i = 0; i < playrs_left; ++i)
			{
				if (*straight_pointer[involved_players[i]] > counter) {	counter = *straight_pointer[involved_players[i]]; }
			}
			for (int i = 0; i < playrs_left; ++i)
			{
				if (*straight_pointer[involved_players[i]] == counter) { still_involved[counter2] = involved_players[i]; ++counter2; }
			}
			playrs_left = counter2;
			if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * still_involved[0]]; break; }
			else {/*tie*/ ++result[0]; ++tie[6]; for (int i = 0; i < playrs_left; ++i) { ++result[2 + 2 * still_involved[i]]; tie[still_involved[i]] += 1. / playrs_left; } }
			break;
		case 6:/*flush*/
			for (int c = 0; c < (cardcnt - 2); ++c) //müssen nur die 5 höchsten karten vergleichen
			{
				counter = 0;
				for (int i = 0; i < playrs_left; ++i)  //for schleife zählt durch involved_players[i];
				{
					compare_cards[i] = sflush[7 * involved_players[i] + (cardcnt - 1) - c];
				}
				int max_value = 0;//let's compare the highest high cards in merged
				for (int i = 0; i < playrs_left; ++i)
				{
					if (compare_cards[i] > max_value) { max_value = compare_cards[i]; }
				}
				for (int i = 0; i < playrs_left; ++i)
				{
					if (compare_cards[i] == max_value) { still_involved[counter] = involved_players[i]; ++counter; }
				}
				for (int i = 0; i < counter; ++i)
				{
					involved_players[i] = still_involved[i];
				}
				playrs_left = counter;
				if ((c == (cardcnt - 3)) && (playrs_left > 1))
				{
					/*tie*/
					++result[0];
					++tie[6];
					for (int i = 0; i < playrs_left; ++i)
					{
						++result[2 + 2 * involved_players[i]];
						tie[involved_players[i]] += 1. / playrs_left;
					}
				}
				if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * involved_players[0]]; break; }
			}
			break;
		case 7:/*full house*/
			   //cnt_zero_grps[p]
			   //max_size_grp[p] immer == 2
			   //suchen und vergleichen erstmal dreiergruppe
			for (int i = 0; i < playrs_left; ++i)
			{
				for (int c = 0; c < 5; ++c)
				{
					if ((dif[6 * involved_players[i] + 5 - c] == 0) && (dif[6 * involved_players[i] + 5 - c - 1] == 0))
					{
						compare_cards[i] = merged[7 * involved_players[i] + 6 - c];
						break;
					}
				}
			}
			counter = 0;
			counter2 = 0;//compare card values of highest trip pairs
			for (int i = 0; i < playrs_left; ++i)
			{
				if (compare_cards[i] > counter2) { counter2 = compare_cards[i]; }
			}
			for (int i = 0; i < playrs_left; ++i)
			{
				if (compare_cards[i] == counter2) { still_involved[counter] = involved_players[i]; ++counter; }
			}
			for (int i = 0; i < counter; ++i)
			{
				involved_players[i] = still_involved[i];
			}

			playrs_left = counter;
			if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * involved_players[0]]; break; }
			//else: find highest next pair. could be trips again!
			for (int i = 0; i < playrs_left; ++i)
			{
				for (int c = 0; c < 6; ++c)
				{
					if ((dif[6 * involved_players[i] + 5 - c] == 0) && (compare_cards[i] != merged[7 * involved_players[i] + 6 - c]))//&& (compare_cards[i] != dif[6*involved_players[i] + 5 - c]))
					{
						compare_cards[i] = merged[7 * involved_players[i] + 6 - c];
						break;
					}
				}
			}
			counter = 0;
			counter2 = 0;
			for (int i = 0; i < playrs_left; ++i)
			{
				if (compare_cards[i] > counter2) { counter2 = compare_cards[i]; }
			}
			for (int i = 0; i < playrs_left; ++i)
			{
				if (compare_cards[i] == counter2) { still_involved[counter] = involved_players[i]; ++counter; }
			}
			for (int i = 0; i < counter; ++i)
			{
				involved_players[i] = still_involved[i];
			}

			playrs_left = counter;
			if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * involved_players[0]]; break; }
			else 
			{ 
				/*tie*/
				++tie[6];
				++result[0];
				for (int i = 0; i < playrs_left; ++i) 
				{ 
					++result[2 + 2 * involved_players[i]];
					tie[involved_players[i]] += 1. / playrs_left;
				}
			}
			break;
		case 8:/*quads*/
			   //find quads
			for (int p = 0; p < playrs_left; ++p)
			{
				for (int c = 0; c < 4; ++c)
				{
					if ((dif[6 * involved_players[p] + 5 - c] == 0) && (dif[6 * involved_players[p] + 5 - c - 1] == 0) && (dif[6 * involved_players[p] + 5 - c - 2] == 0))
					{
						compare_cards[p] = merged[7 * involved_players[p] + 6 - c];
					}
				}
			}
			counter = 0;
			counter2 = 0;
			for (int i = 0; i < playrs_left; ++i)
			{
				if (compare_cards[i] > counter2) { counter2 = compare_cards[i]; }
			}
			for (int i = 0; i < playrs_left; ++i)
			{
				if (compare_cards[i] == counter2) { still_involved[counter] = involved_players[i]; ++counter; }
			}
			playrs_left = counter;

			if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * still_involved[0]]; break; }
			//else: check kicker
			for (int i = 0; i < playrs_left; ++i)
			{
				involved_players[i] = still_involved[i];
			}
			//counter2 is the highest quad card. we'll use that here.
			for (int i = 0; i < playrs_left; ++i)
			{
				for (int c = 0; c < 7; ++c)
				{
					if (merged[7 * involved_players[i] + 6 - c] != counter2) { compare_cards[i] = merged[7 * involved_players[i] + 6 - c]; break; }
				}
			}
			counter = 0;
			counter2 = 0;
			for (int i = 0; i < playrs_left; ++i)
			{
				if (compare_cards[i] > counter2) { counter2 = compare_cards[i]; }
			}
			for (int i = 0; i < playrs_left; ++i)
			{
				if (compare_cards[i] == counter2) { still_involved[counter] = involved_players[i]; ++counter; }
			}
			playrs_left = counter;
			if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * still_involved[0]]; }
			else 
			{ 
				/*tie*/ 
				++result[0];
				++tie[6];
				for (int i = 0; i < playrs_left; ++i) 
				{ 
					++result[2 + 2 * still_involved[i]];
					tie[still_involved[i]] += 1. / playrs_left;
				}
			}
			break;
		case 9:/*sf wheelie*/
			   //only tie possible!
			++result[0];
			++tie[6];
			for (int i = 0; i < playrs_left; ++i)
			{
				++result[2 + 2 * involved_players[i]];
				tie[involved_players[i]] += 1. / playrs_left;
			}
			break;
		case 10:/*sf*/
				//ääääääääääääh wait, wir müssen NUR die HÖCHSTE karte vergleichen!!!
			for (int c = 0; c < (cardcnt - 2); ++c) //müssen nur die 5 höchsten karten vergleichen
			{
				counter = 0;
				for (int i = 0; i < playrs_left; ++i)  //for schleife zählt durch involved_players[i];
				{
					compare_cards[i] = *(sflush_pointer[7 * involved_players[i]] + sf_cnt[involved_players[i]] - c);
					//cout << "player " << 
					//compare_cards[i] = merged[7 * involved_players[i] + (cardcnt - 1) - c];
				}
				int max_value = 0;//let's compare the highest high cards in merged
				for (int i = 0; i < playrs_left; ++i)
				{
					if (compare_cards[i] > max_value) { max_value = compare_cards[i]; }
				}
				for (int i = 0; i < playrs_left; ++i)
				{
					if (compare_cards[i] == max_value) { still_involved[counter] = involved_players[i]; ++counter; }
				}
				for (int i = 0; i < counter; ++i)
				{
					involved_players[i] = still_involved[i];
				}
				playrs_left = counter;
				if ((c == (cardcnt - 3)) && (playrs_left > 1))
				{
					/*tie*/
					++result[0];
					++tie[6];
					for (int i = 0; i < playrs_left; ++i)
					{
						++result[2 + 2 * involved_players[i]];
						tie[involved_players[i]] += 1. / playrs_left;
					}
				}
				if (playrs_left == 1) { ++result[0]; ++result[1 + 2 * involved_players[0]]; break; }
			}
			break;
		default:
			cout << "ERROR IN ORDERING SEKTION" << endl;
		}
		//delete[] compare_cards;
	}
	//cout << rank_cnt[winning_rank] << ' ' << winning_rank << endl;
}

void generate_board(int(&board)[10], int(&hands)[6 * 2 * 2], int cds[52][2], int players)
{
	int *cards[52];
	int *tmp;
	//pointers to card array. later we manipulate the addresses they point to, effectively shuffling our deck of pointers, but not our original deck.
	for (int i = 0; i < 52; ++i)
	{
		cards[i] = &cds[i][0];
	}

	int *deck[52];

	//!!!!!!!!!!!!!!!!!
	//IMPORTANT: It's ok to only do this once, for a given set of player cards. I should move this out of the function, somehow.
	//!!!!!!!!!!!!!!!!!
	//plan is for deck[52] to hold the starting hands first, then the board cards and eventually the leftover deck, to draw more cards from.
	for (int p = 0; p < players; ++p)
	{
		deck[2 * p] = cards[4 * hands[4 * p] + hands[4 * p + 2]];
		cards[4 * hands[4 * p] + hands[4 * p + 2]] = 0;
		deck[2 * p + 1] = cards[4 * hands[4 * p + 1] + hands[4 * p + 3]];
		cards[4 * hands[4 * p + 1] + hands[4 * p + 3]] = 0;
	}

	int count = 0;
	for (int c = 0; c < 52; ++c)
	{
		if (cards[c]) { deck[2 * players + count] = cards[c]; ++count; }
	}

	int board_cards[5]; //write 5 random numbers into
	for (int i = 0; i < 5; ++i)
	{
		//the first 2*players cards are the starting hands. with each new card drawn, we have i less cards to draw from.
		uniform_int_distribution<> dist(i + 2 * players, 51); 
		board_cards[i] = dist(gen);
	}
	
	//swap board cards
	for (int i = 0; i < 5; ++i)
	{
		tmp = deck[i + 2 * players];
		deck[i + 2 * players] = deck[board_cards[i]];
		deck[board_cards[i]] = tmp;
	}

	//write board array, which is used for calculating equity
	for (int i = 0; i < 5; ++i)
	{
		board[i] = *deck[i + 2 * players];
		board[i + 5] = *(deck[i + 2 * players] + 1);
	}
}

void equity(int(&board)[10], int(&hands)[6 * 2 * 2], int(&merged)[cardcnt * player], const int player, int totalhands)
{
	int *cards[52];
	int *tmp;
	//pointers to card array. later we manipulate the addresses they point to, effectively shuffling our deck of pointers, but not our original deck.
}