/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define MAIN_C
#include "main.h"
// current wsys screen
struct widget *screen;
// theme for skier
struct frame *skin;
// game state
a_state state;

void menu(int key)
{
	screen->draw(screen);
	screen->key(screen, key);
}

void play(struct s_skier *skier)
{
	mx_draw();
	mx_service();
	mx_move(skier->vx);
	sk_draw(skier);
	hl_draw(skier);
}

void game_show(void)
{
	sl_cls();
	state = A_training;
}

void quit(void)
{	
	state = A_quit;
}

static int init(void)
{
	char *datadir, *gfxdir, *hilldir;

	// srand(time(0) ^ getpid(0)); 
	rand_init();
	// slang init
	sl_init(NULL);
	// matrix init (background)
	mx_init(0, 0, WIDTH, HEIGHT);	
	// ASCIIJUMP_DATADIR overrides the compiled-in data folder (relocated installs)
	if ((datadir = getenv("ASCIIJUMP_DATADIR")) == NULL)
		datadir = DATADIR;
	gfxdir = strdglue(datadir, "gfx");
	hilldir = strdglue(datadir, "hills");
	// skin init
	if ((skin = skin_make("gfx")) == NULL && (skin = skin_make(gfxdir)) == NULL)
		return 1;
	// read diriectory with hills
	if (hl_readdir("hills") <= 0 && hl_readdir(hilldir) <= 0) {
		printf("no hill !\n");
		return 2;
	}
	// init skiers list, used in world cup
	sk_array_init(cup_sk_limit);
	// setting-up global (cup) pointers, i don`t want to lose this list
	cup_sk_array = sk_array;
	cup_sk_head = sk_head;
	cup_sk_tail = sk_tail;
	// init training skier
	tr_skier = sk_init(hl_head, skin, "foo");
	sk_change_mode(tr_skier, NULL, Blue, 0, 1);

	mn_scr_show();

	return 0;
}

static int resize()
{
	struct s_skier *s;
	
	if (sl_screen_size_changed == 0)
		return 0;

	sl_cls();
	
	if (WIDTH <= 40 || HEIGHT <= 16) 
		return 1;
	
	sl_fini();
	sl_init(NULL);
	sl_screen_size_changed = 0;
	
	for (s = sk_head; s; SWITCH(s)) {
		s->current_x =  WIDTH/2-4;
		s->current_y =  HEIGHT/2-2;
	}
	
	tr_skier->current_x =  WIDTH/2-4;
	tr_skier->current_y =  HEIGHT/2-2;
	
	mx_free();
	mx_init(0, 0, WIDTH, HEIGHT);
	
	return 0;
}

a_state loop()
{
	int key;
	if (resize())
		return 1;
	
	key = sl_getch2();
	
	switch (state) {
	case A_client:
		client(key);
		break;
		
	case A_menu:
		menu(key);
		break;
		
	case A_training:
		if (sk_service(tr_skier, key)) 
			play(tr_skier);
		else {
			sl_cls();
			state = A_menu;
		}
		
		break;
	case A_cup:
		cup(key);
		break;
		
	case A_quit:
		break;
		
	}

	usleep(0xdead - 0xcafe/0x6);
	sl_refresh();
	
	return state;
}

int main(int argc, char **argv)
{
	int ret = 0;

	if (parse_cmdline(argc, argv) == 0)
		goto asciijump_end;

	if ((ret = init()) != 0) 
		goto asciijump_end;
		
	intro();
	
	for (; loop() != A_quit;);
	
	asciijump_end:
	
	sl_fini();
	return ret;		
}
