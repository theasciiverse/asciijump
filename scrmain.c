/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define SCRMAIN_C
#include "scrmain.h"

char *mn_help_text = 
"\n"
" :: asciijump project\n\n"
"    shortcuts  ::\n"
"    enter -  jump\n"
"    enter -  telemark\n"
"    right -  move right                  ::\n"
"    left  -  move left\n"
"    up    -  scroll menu up              ::\n"
"    down  -  scroll menu down            ::\n"
"    space -  select position\n"
"    tab   -  next widget\n"
"    q     -  leave hill\n\n\n"
" ::  copyright (c) 2003,\n"
" ::  grzegorz moskal, eldevarth@nemerle.org\n\n"
"     michal moskal, malekith@pld-linux.org\n"
" ::  przemyslaw niezborala, finburson@o2.pl";

static struct widget* mn_scr;
static struct widget* mn_win;
static struct widget* mn_menu;
static struct widget* mn_win_help;

void mn_scr_show(void)
{
	if (mn_scr == NULL)
		mn_scr_init();
		
	state = A_menu;
	screen = mn_scr;
	mn_scr->current = mn_win;
	screen_hideall(mn_scr);
}

static void mn_help_show()
{
	mn_win_help->hidden = 0;
	mn_scr->current = mn_win_help;
	sl_cls();
}

static void mn_scr_init(void)
{
	int uw = WIDTH/10;
	int uh = HEIGHT/10;
	
	mn_scr = screen_add();
	
	mn_win = window_add(mn_scr, PACKAGE " game v " VERSION, 1, 1, uw*6, 10);
	mn_menu = menu_add(mn_win, 1, 1, uw*6-2, 10, 0);

	menuobj_add(mn_menu, "world cup", 'w', cup_scr_show, OBJ_DOWN);
	menuobj_add(mn_menu, "training", 't',tr_scr_show, 0);
	menuobj_add(mn_menu, "join a game", 'j', cl_scr_show, 0);
	menuobj_add(mn_menu, "help", 'h', mn_help_show, 0);
	menuobj_add(mn_menu, "quit", 'q', quit, OBJ_DOWN);
	
	mn_win_help = window_add(mn_scr, "_he_lp", 5, 0, 49, 24);
	menuobj_add(menu_add(mn_win_help, 1, 22, 47, 3, 0), "<<(",'(', mn_scr_show, 0);
	mn_win_help->hidden = 1;
	textbox_add(mn_win_help, mn_help_text);
}
