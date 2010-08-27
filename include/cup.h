#ifndef CUP_H
#define CUP_H

#ifdef  CUP_C

#define JUMPER (cup_sk_array[cup_sk_no])
#define HILL (cup_hl_array[cup_hl_no])

#include <string.h>	// strlen()
#include <stdio.h>	// sprintf()
#include "slangwrap.h"	// WIDTH, sl_cls() ..
#include "xfnc.h"	// xmalloc()
#include "scrmain.h"	// scrmain_show()
#include "scrcup.h"	// scrcup_init()
#include "scrresult.h"	// scrcup_init()
#include "intro.h"	// typewriter()
#include "skier.h"	// struct s_skier
#include "hill.h"	// struct hill
#include "main.h"	// struct hill *hills
#include "wsys.h"	// text window system.

#else
extern struct s_skier *cup_sk_head, *cup_sk_tail;
extern struct s_skier **cup_sk_array;
extern int cup_hl_counter;
extern int cup_sk_counter;
extern int cup_sk_no;
extern int cup_hl_no;
extern struct s_hill **cup_hl_array;
extern int cup_status;
extern int cup_sk_limit;
#endif
void cup_show();
void cup(int k);

#endif
