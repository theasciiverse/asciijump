#ifndef MAIN_H
#define MAIN_H
typedef enum asciijump_state {
A_menu,
A_training,
A_cup,
A_client,
A_quit
} a_state;
#ifdef  MAIN_C
#include <unistd.h>	// usleep()
#include "skier.h"	// sk_draw()
#include "hill.h"	// hl_draw()
#include "skin.h"	// struct skin
#include "slangwrap.h"	// sl_init(), WIDTH ..
#include "matrix.h"	// mx_init()
#include "scrmain.h"	// mn_init()
#include "scrtrain.h"	// tr_init()
#include "cup.h"	// cup()
#include "intro.h"	// intro()
#include "wsys.h"	// text window system.
#include "xfnc.h"	// rand_init()
#include "cmdline.h"	// parse_cmdline();
#include "scrtrain.h"	// tr_skier
#include "client.h"	// client()
static void game(int key);
static int init(void);
static int resize(void);
#else 
extern struct widget *screen;
extern struct hill *hills;
extern struct frame *skin;
extern a_state state;
#endif

struct s_skier;
void menu(int key);
void play(struct s_skier *skier);
void game_show(void);
void quit(void);
int main(int argc, char **argv);
#endif
