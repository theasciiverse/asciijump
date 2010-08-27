#ifndef SCRTRAIN_H
#define SCRTRAIN_H

#ifdef  SCRTRAIN_C
#include <string.h>	// strlen()
#include "skier.h"	// struct object
#include "hill.h"	// struct hill
#include "main.h"	// game_show()
#include "scrmain.h"	// scrmain_show()
#include "slangwrap.h"	// WIDTH ..
#include "xfnc.h"	// SWITCH macro
#include "wsys.h"	// text window system.
static void tr_usehill(void);
static void tr_scr_init(void);
#else
struct s_skier;
extern struct s_skier *tr_skier;
#endif
void tr_scr_show(void);
#endif
