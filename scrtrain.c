/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define SCRTRAIN_C
#include "scrtrain.h"

struct s_skier *tr_skier;
static struct widget* tr_scr;
static struct widget* tr_win;
static struct widget* tr_menu;

void tr_scr_show()
{
	if (tr_scr == NULL) 
		tr_scr_init();
	
	screen = tr_scr;
	state = A_menu;
	sl_cls();
}

static void tr_usehill(void)
{
	char *title = menu_selected_caption(tr_menu);
	struct s_hill *hl = hl_find(title, NULL);
	
	sk_flush(tr_skier, hl);

	game_show();		// GAME SHOW !! ugh !!! TODO
}

static void tr_scr_init()
{
	struct s_hill *h = hl_head;

	int uw = WIDTH/10;
	int uh = HEIGHT/10;
	
	tr_scr = screen_add();
	tr_win = window_add(tr_scr, "select _h_i_l_l", 1, 1, uw*4, uh*10-1);
	tr_menu = menu_add(tr_win, 1, 2, uw*4-2, uh*10-3, 0);
	
	for (; h; SWITCH(h))
		menuobj_add(tr_menu, h->name,
			h->name[d(strlen(h->name))-1], tr_usehill, 0);

	menuobj_add(tr_menu, "<<(",'(', mn_scr_show, OBJ_DOWN);
}
