#ifndef SCRRESULT_H
#define SCRRESULT_H
#ifdef  SCRRESULT_C
#include <string.h>	// strdup()
#include "skier.h"	// struct s_skier
#include "hill.h"	// struct s_hill
#include "main.h"	// int status
#include "slangwrap.h"	// WIDTH, sl_cls() ..
#include "wsys.h"	// text window system.
#include "cup.h"	// struct s_skier **cup_sk_array
#include "xfnc.h"	// strglue()
#include "scrresult.h"	// rs_action()
static void rs_scr_reinit_winners(struct s_skier **array, int length);
static void rs_scr_reinit_classic(struct s_skier **array, int length, struct s_hill *hl);
#endif
struct s_skier;
struct s_hill;
void rs_actions(void (*classic)(void), void (*winners)(void));
void rs_scr_reinit(struct s_skier **array, int length, struct s_hill *hl);
#endif
