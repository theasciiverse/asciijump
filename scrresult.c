/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define SCRRESULT_C
#include "scrresult.h"

static struct widget *rs_scr;
static struct widget *rs_win;
static struct widget *rs_menu;

static void (*rs_fnc_classic)(void);
static void (*rs_fnc_winners)(void);

void rs_actions(void (*classic)(void), void (*winners)(void))
{
	rs_fnc_classic = classic;
	rs_fnc_winners = winners;
}

static void rs_scr_reinit_winners(struct s_skier **array, int length)
{
	int w = WIDTH/10*4;
	int h = HEIGHT-2;

	int i, place = 0, len = 0, tmplen = 0;
	char *text = NULL;
	
	rs_win = window_add(rs_scr, " _winner_s ", 1, 1, w, h-1);
	
	for (i = length-1; i+1; i--) {
		if (array[i] == NULL)
			continue;
			
		if (text)
			tmplen = strlen(text);

			
		len += strlen(array[i]->name) + 20;
		text = xrealloc(text, sizeof(char) * len);
		sprintf(text + tmplen, "  %d. %.1f  %s\n",
			++place, array[i]->points, array[i]->name);
	}
	
	textbox_add(rs_win, text);
	xfree(text);	
	
	menuobj_add(menu_add(rs_win, 1, h-3, w-2, 1, 0), "<<(", '(', rs_fnc_winners, 0);
}

static void rs_scr_reinit_classic(struct s_skier **array, int length, struct s_hill *hl)
{
	int i;
	int h = HEIGHT-1;
	int w = WIDTH/10*8+3;
	
	struct s_skier *s;
	struct widget *mo;
	char *name = strglue("results _(next hill: ", hl->name, " _)");
	
	rs_win = window_add(rs_scr, name, 1, 1, w, h-1);
	xfree(name);

	rs_menu = menu_add(rs_win, 1, 1, w-2, h-2, 0);
	rs_menu->selectable = 0;
	
	for (i = length-1; i+1; i--) {
		if ((s = array[i]) == NULL)
			continue;
			
		name = xmalloc(sizeof(char) * (strlen(s->name) + 30));
		sprintf(name, "%s  --  %.1f(%.1f)", s->name, s->points,	s->last_points);
		mo = menuobj_add(rs_menu, name, name[d(strlen(name)-2)], NULL, 0);

		xfree(name);
	}
		
	menuobj_add(menu_add(rs_win, 1, h-3, w-2, 1, 0), ")>>", ')', rs_fnc_classic, 0);
}

void rs_scr_reinit(struct s_skier **array, int length, struct s_hill *hl)
{
	if (array == NULL)
		rs_fnc_winners();
	
	if (rs_scr == NULL)
		rs_scr = screen_add();
		
	rs_win = window_free(rs_win);
	rs_menu = menu_free(rs_menu);
		
	if (hl)
		rs_scr_reinit_classic(array, length, hl);
	else
		rs_scr_reinit_winners(array, length);

	rs_scr->kids = rs_scr->current = rs_win;
	sl_cls();
	state = A_menu;
	screen = rs_scr;
}
