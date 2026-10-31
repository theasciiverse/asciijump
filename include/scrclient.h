#ifndef SCRCLIENT_H
#define SCRCLIENT_H

#ifdef  SCRCLIENT_C
#include <string.h>	// strlen()
#include <stdlib.h>
#include "main.h"	// game_show()
#include "scrmain.h"	// scrmain_show()
#include "slangwrap.h"	// WIDTH ..
#include "xfnc.h"	// SWITCH macro
#include "wsys.h"	// text window system.
#include "client.h"	// cl_name,
static void cl_winstatus_init(void);
#endif
void cl_scr_show(void);
void cl_scr_init(void);
void cl_winstatus_show(void);

void cl_winstatus_cleanup(void);
void cl_winstatus_putd(int digit);
void cl_winstatus_putc(char c);
void cl_winstatus_puts(char *text);
#endif
