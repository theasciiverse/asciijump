/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define CUP_C
#include "cup.h"

struct s_skier *cup_sk_head, *cup_sk_tail;
struct s_skier **cup_sk_array;
// index for skiers array
int cup_sk_no;
// current size for skiers array
int cup_sk_counter;
// max size for skiers array
int cup_sk_limit = 8;

// hills array,
struct s_hill **cup_hl_array;
// current size for skiers array
int cup_hl_counter;

int cup_hl_no;

void cup_show()
{
	state = A_cup;
}

void cup(int key)
{
	int i = 0;
	
	if (cup_sk_no == cup_sk_counter) {
		rs_scr_reinit(cup_sk_array, cup_sk_counter, HILL);
		cup_sk_no = 0;
		return;
	}
	// if this skier is controled by human and there is still some move on the screen
	// we have to wait for the finish. (may he plays)
	if (JUMPER->level == 0 && sk_service(JUMPER, key)) {
		play(JUMPER);	
		return;
	}
	
	if (JUMPER->level != 0)
		sk_cpujump(JUMPER);
	
	// next one please ;)
	cup_sk_no++;
	// if he isn`t out of bond, and .. there is still some hill
	if (cup_sk_no >= cup_sk_counter && HILL) {
		cup_hl_no++;
		for (i = 0; i < cup_sk_counter; i++) 
			sk_flush(cup_sk_array[i], HILL);
		sk_array_sort();
	}
}
