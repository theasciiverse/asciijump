#ifndef SCRMAIN_H
#define SCRMAIN_H

#ifdef  SCRMAIN_C
#include <stdlib.h>	// NULL
#include "scrclient.h"	// cl_scr_show()
#include "skier.h"	// struct object
#include "scrtrain.h"	// scrtrain_show
#include "main.h"	// int status, struct object *screen
#include "scrcup.h"	// scrcup_show
#include "slangwrap.h"	// WIDTH, HEIGHT
#include "wsys.h"	// text window system
static void mn_help_show(void);
static void mn_scr_init(void);
#endif
void mn_scr_show(void);
#endif
