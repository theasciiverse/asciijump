/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define SCRCUP_C
#include "scrcup.h"

static struct widget* cup_scr;

static struct widget* cup_win_main;
static struct widget* cup_menu_main;

static struct widget* cup_win_hill;
static struct widget* cup_menu_hill;

static struct widget *cup_win_skier;
static struct widget *cup_menu_skier;

static struct widget *cup_win_select;
static struct widget *cup_line_selectname; 
static struct widget *cup_menu_selectcontroler;
static struct widget *cup_menu_selectcolor;
static struct widget *cup_menu_selectlevel;

static struct s_skier *sk_cup_current;


void cup_scr_show(void)
{
	if (cup_scr == NULL)
		cup_scr_init();

	cup_scr->current = cup_win_main;
	screen_hideall(cup_scr);
	state = A_menu;
	screen = cup_scr;

	sk_array = cup_sk_array;
	sk_head = cup_sk_head;
	sk_tail = cup_sk_tail;
}

static void cup_winmain_show()
{
	cup_scr->current = cup_win_main;
	screen_hideall(cup_scr);
}

static void cup_winhill_show()
{
	cup_scr->current = cup_win_hill;
	cup_win_hill->hidden = 0;
	screen_hideall(cup_scr);
}

static void cup_winskier_show()
{
	cup_scr->current = cup_win_skier;
	cup_win_skier->hidden = 0;
	screen_hideall(cup_scr);
}

static void cup_winskier_refresh()
{
	inputline_fill(cup_menu_skier->current, sk_cup_current->name);
	menu_tick(cup_menu_skier->current);
	cup_winskier_show();
}

static void cup_winselect_show(void)
{
	cup_win_select->hidden = 0;
	cup_scr->current = cup_win_select;
	screen_hideall(cup_scr);
}

static void cup_winselect_refresh()
{
	
	screen_hideall(cup_scr);
	
	sk_cup_current = sk_rewind(menu_selected_number(cup_menu_skier));
	inputline_fill(cup_line_selectname, sk_cup_current->name);
	
	menu_select(cup_menu_selectcolor, sk_cup_current->color-2);	
	menu_select(cup_menu_selectcontroler, sk_cup_current->level);
	
	cup_menu_selectlevel->hidden = 1;
	cup_menu_selectcolor->hidden = 1;
	
	if (sk_cup_current->level) {
		menu_select(cup_menu_selectlevel, sk_cup_current->level-1);	
		cup_menu_selectlevel->hidden = 0;
	} else 
		cup_menu_selectcolor->hidden = 0;
		
	cup_win_select->current = cup_line_selectname;
	cup_winselect_show();
}

static void cup_set_color()
{
	sk_cup_current->color = menu_selected_number(cup_menu_selectcolor) + 2;
	list_switch(cup_win_select, NEXT_WIDGET);
}

static void cup_set_controler()
{
	sk_cup_current->level = menu_selected_number(cup_menu_selectcontroler);
	cup_menu_selectlevel->hidden = 1;
	cup_menu_selectcolor->hidden = 1;
	if (sk_cup_current->level) 
		cup_menu_selectlevel->hidden = 0;
	else {
		cup_menu_selectcolor->hidden = 0;
		sl_cls();
	}
	list_switch(cup_win_select, NEXT_WIDGET);
}

static void cup_set_name()
{
	xfree(sk_cup_current->name);
	sk_cup_current->name = strdup(inputline_caption(cup_line_selectname));
}

static void cup_set_level()
{
	sk_cup_current->level += menu_selected_number(cup_menu_selectlevel);
	list_switch(cup_win_select, NEXT_WIDGET);
}

static void cup_init(void)
{
	int i;
	struct widget *mo = cup_menu_skier->kids;
	char *title;
	// clean up skiers array
	for (i = 0; i < cup_sk_limit; i++)
		cup_sk_array[i] = NULL;
	// now fill up skiers array	(by selected mune items)
	for (mo = cup_menu_skier->kids, i = 0; mo; SWITCH(mo)) {
		if ((mo = menu_ticked(mo)) == NULL)
			break;
		cup_menu_skier->current = mo;
		
		cup_sk_array[i++] = sk_rewind(menu_selected_number(cup_menu_skier));
	}
	// set up number of skiers in this competition.
	cup_sk_counter = i-1;
	
	if (cup_hl_array == NULL)
		cup_hl_array = xmalloc(sizeof(struct s_hill *) * hl_counter);	
	// clean up hills array
	for (i = 0; i < hl_counter; i++)
		cup_hl_array[i++] = NULL;
	// and now fill up this array	(by selected menu items)
	for (mo = cup_menu_hill->kids, i = 0; mo; SWITCH(mo)) {
		if ((mo = menu_ticked(mo)) == NULL)
			break;
		cup_menu_hill->current = mo;
		
		title = menu_selected_caption(cup_menu_hill);
		cup_hl_array[i++] = hl_find(title, NULL);	// NULL mean standard hills.
	}
	// set up number of hills in this competition.
	cup_hl_counter = i-1;
	
	if (cup_sk_array[0] && cup_hl_array[0]) {
		cup_sk_no = 0;
		cup_hl_no = 0;
		
		for (i = 0; i < cup_sk_counter; i++) {
			cup_sk_array[i]->points = 0;
			cup_sk_array[i]->hill = cup_hl_array[0];
			
		}

		rs_actions(cup_show, mn_scr_show);

		sk_array = cup_sk_array;
		sk_head = cup_sk_head;
		sk_tail = cup_sk_tail;

		state = A_cup;
	}
}

static void cup_scr_init(void)
{
	int w = WIDTH/10*8+2;
	int h = 10;
	
	cup_scr = screen_add();
	cup_win_main = window_add(cup_scr, "world cup", 1, 1, w, h);
	cup_menu_main = menu_add(cup_win_main, 1, 2, w-2, h-3, 0);

	
	menuobj_add(cup_menu_main, "select competitors", 'c', cup_winskier_show, 0);
	menuobj_add(cup_menu_main, "select hills", 'h',cup_winhill_show, 0);
	menuobj_add(cup_menu_main, "start world cup", 't', cup_init, OBJ_DOWN);
	menuobj_add(cup_menu_main, "<<(", '(', mn_scr_show, OBJ_DOWN);
	
	cup_winselect_init();
	cup_winskier_init();
	cup_winhill_init();
}

static char *colors[] = {
	"grey", "white", "bright white", "green", "bright green",
	"cyan", "bright cyan","red","bright red", "blue", "bright blue", 
	"magenta", "bright magenta", "brown","yellow", NULL
};

static void cup_winselect_init(void)
{
	int i = 0;
	int w = WIDTH/10*8+3;
	int h = HEIGHT-1;
	// init win select
	cup_win_select = window_add(cup_scr, "setup", 1, 0, w, h);
		
	// type your name
	label_add(cup_win_select, 1, 1, 10, "name: ");
	cup_line_selectname = inputline_add(cup_win_select, 11, 1, w-2, cup_set_name);

	// select if cpu player
	cup_menu_selectcontroler = menu_add(cup_win_select, 1, 3, w/3, 2, 0);
	menuobj_add(cup_menu_selectcontroler, "human",'h', cup_set_controler, 0);
	menuobj_add(cup_menu_selectcontroler, "cpu", 'u',cup_set_controler, 0);
	
	// select colors
	cup_menu_selectcolor = menu_add(cup_win_select, w/2, 3, w/2, 15, 0);
	for (; colors[i]; i++)
		menuobj_add(cup_menu_selectcolor, colors[i],
			colors[i][d(strlen(colors[i]))-1], cup_set_color, 0);		
			
	// selcet level
	cup_menu_selectlevel = menu_add(cup_win_select, w/2, 3, w/2-2, 4, 0);
	menuobj_add(cup_menu_selectlevel, "poor",'p', cup_set_level, 0);
	menuobj_add(cup_menu_selectlevel, "good", 'o', cup_set_level, 0);
	menuobj_add(cup_menu_selectlevel, "best", 'b', cup_set_level, 0);

	// back to skier window
	menuobj_add(menu_add(cup_win_select, 1, h-2, w-2, 1, 0), 
		"<<(", '(', cup_winskier_refresh, 0);
}
	
static void cup_winskier_init(void)
{
	struct s_skier *s;
	
	int w = WIDTH/10 * 6 + 3;
	int h = cup_sk_limit + 5; 
	
	cup_win_skier = window_add(cup_scr, "_select _competitors", 1, 1, w, h);
	cup_menu_skier = menu_add(cup_win_skier, 1, 2, w-2, h-3, 1);
	
	for (s = cup_sk_head; s; SWITCH(s))
		menuobj_add(cup_menu_skier, s->name, s->name[0], cup_winselect_refresh, 0);
		
	menuobj_add(cup_menu_skier, "<<(", '(', cup_winmain_show, OBJ_DOWN+OBJ_UNTICKABLE);
}

static void cup_winhill_init(void)
{
	struct s_hill *hj;
	
	int w = WIDTH/10 * 6 + 3;
	int h = HEIGHT-1; 
	
	cup_win_hill = window_add(cup_scr, "se_lect hi_l_l", 1, 1, w, h);
	cup_menu_hill = menu_add(cup_win_hill, 1, 2, w-2, h-3, 1);
	
	for (hj = hl_head; hj; SWITCH(hj))
		menuobj_add(cup_menu_hill, hj->name, 
			hj->name[d(strlen(hj->name))-1], 0, 0);
		
	menuobj_add(cup_menu_hill, "<<(",'(', cup_winmain_show, OBJ_DOWN+OBJ_UNTICKABLE);
}
