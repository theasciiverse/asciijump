#ifndef SCRCUP_H
#define SCRCUP_H
#ifdef  SCRCUP_C
#include <string.h>	// strdup()
#include "skier.h"	// struct jumper
#include "hill.h"	// struct hill
#include "main.h"	// int status
#include "scrmain.h"	// scrmain_show()
#include "scrresult.h"
#include "slangwrap.h"	// WIDTH, sl_cls() ..
#include "wsys.h"	// text window system.
#include "xfnc.h"	// SWITCH macro, xfree()
#include "cup.h"	// struct object *jumpers
static void cup_winmain_show(void);
static void cup_winhill_show(void);
static void cup_winskier_show(void);
static void cup_winskier_refresh(void);
static void cup_winselect_show(void);
static void cup_winselect_refresh(void);
static void cup_set_color(void);
static void cup_set_controler(void);
static void cup_set_name(void);
static void cup_set_level(void);
static void cup_init(void);
static void cup_scr_init(void);
static void cup_winselect_init(void);
static void cup_winskier_init(void);
static void cup_winhill_init(void);
#endif
void cup_scr_show(void);
#endif
