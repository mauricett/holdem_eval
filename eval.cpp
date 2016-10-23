#include <iostream>
#include <string>
#include <algorithm>

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

constexpr int player = 6; //max values for player and cardcnt
constexpr int cardcnt = 7;

int merged[cardcnt * player]; //merged starting hands and board, without suits

void find_winner(int(&board)[10], int(&hands)[6 * 2 * 2], int(&merged)[cardcnt * player], const int player)
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

	int player_ranks[6 * 11] = {}; //all ranks a player achieved, 11 different ranks, max 6 players:
	int rank_cnt[11] = {}; //0 high card, 1 pair, 2 twopair, 3 trips, 4, wheelie, 5 straight, 6 flush, 7 fh, 8 quads, 8 wheelie sf, 10 sf

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
					++player_ranks[11 * i + 6];
					++rank_cnt[6];
				}
			}
			break;
		case 4:
			for (int i = 0; i < player; ++i)
			{
				if ((hands[4 * i + 2] == max_suit) || (hands[4 * i + 3] == max_suit))
				{
					++player_ranks[11 * i + 6];
					++rank_cnt[6];
				}
			}
			break;

		case 5:
			for (int i = 0; i < player; ++i)
			{
				++player_ranks[11 * i + 6];
				++rank_cnt[6];
			}
			break;

		default:
			cout << "FEHLER IN FLUSH SEKTION" << endl;
		}
	}
	
	//mit cnt_zero_grps und max_size_grp lassen sich alle pair ranks finden
	int cnt_zero_grps[6] = {}; //wieviele gruppen von nullen, d.h. nullen getrennt durch >0 ?
	int max_size_grp[6] = {}; //wie groß ist größte gruppe?
	int dif[6 * 6] = {};
	int straight[6] = {};
	int str_tmp[6] = {};
	int max_tmp[6] = {}; //tmp zum zwischenspeichern und vergleichen der max-größe
	int *straight_poiner[6];

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
			dif[i + 6*p] = merged[i + 1 + 7*p] - merged[i + 7*p];
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
					if (straight[p] >= 4) { straight_poiner[p] = &(merged[7 * p + i + 1]); } 	//pointer auf letzte straight-karte!
					break;
				default:
					max_tmp[p] = 0;
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
					break;
				default:
					max_tmp[p] = 0;
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
		if ((merged[7*p] == 0) && (merged[6 + 7*p] == 12) && ((dif[6*p] == 0) || (dif[6*p] == 1)) && ((dif[6*p + 1] == 0) || (dif[6*p + 1] == 1)) && ((dif[6*p + 2] == 0) || (dif[6*p + 2] == 1)))
		{
			//wheelie suchen. dass 2 und A in der hand sind, steht schon fest, müssen 3, 4, 5 überprüfen. ... d.h. in unserer nummerierung 1, 2, 3!
			int rest[3] = {}; //buckets für 1, 2 und 3. alle TRUE?
			for (int i = 1; i < (7 - 1); ++i)
			{
				switch (merged[i + 7*p])
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
			if (rest[0] && rest[1] && rest[2]) { wheelie[p] = 1; }
		}
		
		cout << endl << endl << "Player " << p + 1 << ":" << endl;

		if ((straight[p] < 4) && (wheelie[p] == 0)) { cout << "keine straight!" << endl; }
		else if (straight[p] >= 4) { cout << "straight!" << endl; }
		else if (wheelie[p] == 1) { cout << "wheelie!" << endl; }

		if (player_ranks[p*11 + 6]) { cout << "flush!" << endl; }
		if (!(player_ranks[p * 11 + 6])) { cout << "kein flush!" << endl; }

		//remember, haben: int *straight_poiner[6]; um straight zu checken. pointer auf höchstes element der straight

		switch (max_size_grp[p])
		{
		case 0:
			++rank_cnt[0];
			player_ranks[p * 11] = 1;
			cout << "high card" << endl;
			break;
		case 1:
			if (cnt_zero_grps[p] == 1) { ++rank_cnt[1]; player_ranks[p * 11 + 1] = 1; cout << "pair" << endl; }
			else { ++rank_cnt[2]; player_ranks[p * 11 + 2] = 1; cout << "two pair" << endl; }
			break;
		case 2:
			if (cnt_zero_grps[p] == 1) { cout << "trips" << endl; }
			else { cout << "full house" << endl; }
			break;
		case 3:
			cout << "quads" << endl;
			break;
		default:
			cout << "error in pair section" << endl;
			break;
		}

		//remember, haben: int *straight_poiner[6]; um straight zu checken. pointer auf höchstes element der straight

		if (((straight[p] >= 4 || wheelie[p]) && player_ranks[p * 11 + 6]))
		{
			switch (wheelie[p])
			{
			case 0:
				cout << "straight oder royal flush?" << endl;
				break;
			case 1:
				cout << "wheelie straight flush bzw straight flush?" << endl;	//müssen hier den sonderfall berücksichtigen, dass 012345A einen wheelie und eine straight hat, aber die straight höher ist
				if (straight[p] >= 4) { /*check wheelie sf and normal sf. remember, normal sf > wheelie sf*/ }
				else { /*only need to check wheelie sf*/ }
				break;
			default:
				cout << "FEHLER IN STRAIGHT FLUSH SEKTION" << endl;
				break;
			}
		}

	}

	int rank = 10;
	int winning_rank = 0;

	//find winning rank (pair, twopair, etc... 10 different ranks!)
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
	int *compare_cards = new int[rank_cnt[winning_rank]];
	int playrs_left = rank_cnt[winning_rank];

	if (rank_cnt[winning_rank] == 1) //only one player with winning rank
	{
		for (int p = 0; p < player; ++p)
		{
			if (player_ranks[p * 11 + winning_rank] == 1) { cout << "player " << p << " gewinnt mit rank " << winning_rank << endl; break; }
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
			counter = 0;
			for (int c = 0; c < (cardcnt - 2); ++c) //müssen nur die 5 höchsten karten vergleichen
			{
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
					cout << endl << endl << endl << "tie zwischen player " << endl;
					for (int i = 0; i < playrs_left; ++i)
					{
						cout << involved_players[i] << ', ' << endl;
					}
				}
				if (playrs_left == 1) { cout << endl << endl << endl << "winner ist player " << involved_players[0]; break; }
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
			if (playrs_left == 1) { cout << "winning pair hat  " << involved_players[0]+1 << endl; break; }
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
					if ((c == 2) && (playrs_left > 1)) // c==2 means we're in the last loop of for, meaning looking at the lowest of the 5 cards
					{
						/*tie*/
						cout << endl << endl << endl << "tie zwischen player " << endl;
						for (int i = 0; i < playrs_left; ++i)
						{
							cout << involved_players[i] << ", ";
						}
						cout << endl;
					}
					if (playrs_left == 1) { cout << endl << endl << endl << "winner ist player " << involved_players[0]; break; }
				}
			}
			break;
		case 2:/*twopair*/
			//find first pair and filter out losers
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

			if (playrs_left == 1) { cout << "jemand hat höchstes erstes paar - player " << involved_players[0] + 1 << endl; break; }
			//else, zweite pairs vergleichen
			counter = 0;
			counter2 = 0;
			max_pair = 0;
			for (int p = 0; p < playrs_left; ++p)
			{
				for (int c = 5; c >= 0; --c)
				{
					if ((dif[c + 6 * involved_players[p]] == 0) && (counter2 == 0)) { ++counter2; }
					if ((dif[c + 6 * involved_players[p]] == 0) && (counter2 == 1)) 
					{
						pointer_two_pair[p] = &(merged[7 * involved_players[p] + c]); //pointer auf die erste karte des zweiten paars
						break;
					}
				}
				if (*pointer_two_pair[p] > max_pair) { max_pair = *pointer_two_pair[p]; }
			}
			for (int p = 0; p < playrs_left; ++p)
			{
				if (*pointer_two_pair[p] == max_pair) { still_involved[counter] = involved_players[p]; ++counter; }
			}
			playrs_left = counter;
			if (playrs_left == 1) { cout << "jemand hat höchstes zweites paar - player " << involved_players[0] + 1 << endl; break; }
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
			if (playrs_left == 1) { cout << "twopair winner mit bestem kicker " << involved_players[0] + 1 << endl; }
			else 
			{ 
				cout << "tie between players ";
				for (int i = 0; i < playrs_left; ++i)
				{
					cout << involved_players[i] + 1 << ' ';
				}
			}
			break;
		case 3:/*trips*/
			break;
		case 4:/*wheelie*/
			break;
		case 5:/*straight*/
			break;
		case 6:/*flush*/
			break;
		case 7:/*full house*/
			break;
		case 8:/*quads*/
			break;
		case 9:/*sf wheelie*/
			break;
		case 10:/*sf*/
			break;
		default:
			cout << "ERROR IN ORDERING SEKTION" << endl;
		}
		delete[] compare_cards;
	}





	//!!!!!!!!!!!!!!!!
	//WICHTIG!!!!!!!!!
	//EXPERIMENT DAS KLAPPEN MUSS: 012345A mit straight flush (kein wheelie sf), normaler sf muss gewinnen!
	//!!!!!!!!!!!!!!!!
	//!!!!!!!!!!!!!!!!
}


int main()
{
	int board[10]{ 0, 1, 0, 2, 1, 3, 10, 3, 11, 3 }; //  = { (rank1, suit1), (rank2, suit2), ...}; // die 5 board cards, (0,1) == "2d", usw usw
	int board_alt[10]{ 0,5, 4, 1, 2, 3, 3, 3 , 0, 3 }; // = { (rank1, rank2 ...), (suit1, suit2 ...) }; // glaube ist besser, weil wir ja sortieren müssen
	int hands[6 * 2 * 2]{ 6, 0, 6, 1 }; // (6,0),(6,1) ist hand von player#1, usw usw
	int hands_alt[6 * 2 * 2]{ 6, 6, 0, 1 }; // (rank1, rank2, ...), (suit1, suit2 ...), ...
	int *cards[52];


	for (int i = 0; i < 52; ++i)
	{
		cards[i] = &cds[i][0];
	}
	//Evtl interessant für Shuffle.
	//Scheint zu funktionieren (sollt ich aber noch testen):
	cards[36] = cards[3];


	//ACHTUNG: hände müssen in reihenfolge rank1 < rank2 eingetragen werden!
	int hand1[6 * 2 * 2] = {3,12,3,1,2,4,1,1,6,7,2,3};
	/*eval_ranks(hand1, board_alt, merged, 3);
	cout << endl;		*/


	/*rank_players(board_alt, hand1, merged, 3);


	for (int p = 0; p < 3; ++p)
	{
		cout << "Player " << p + 1 << "'s Deck:  ";
		for (int i = 0; i < 7; ++i)
		{
			cout << merged[i + 7 * p] << ' ';
		}
		cout << endl;
	}*/

	int board1[10] = { 1, 3, 1, 3, 5, 0, 0, 1, 1, 3 };
	int hands1[24] = { 4, 5, 1, 2, 4, 5, 2, 2, 10, 12, 3, 3, 3, 5, 2, 2 };

	find_winner(board1, hands1, merged, 4);

	cout << endl << "merged :" << endl;
	for (int p = 0; p < 4; ++p)
	{
		cout << "Player " << p+1 << "'s Deck:  ";
		for (int i = 0; i < 7; ++i)
		{
			cout << merged[i + 7 * p] << ' ';
		}
		cout << endl;
	}

	getchar();
	return 0;
}
