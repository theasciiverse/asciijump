#ifndef AS_DISPLAY_H
#define AS_DISPLAY_H

#ifdef AS_DISPLAY_C
#include <stdio.h>	// printf()
#include <stdlib.h>	// exit()
#include <stdarg.h>	// va_ ... macros
#include <string.h>
#else
extern int as_allow_colored_prompt;
extern int as_debug;
#endif
void as_display(char *, char *, ...);
void as_eerr(char *, int);
void as_prompt(void);
#include "prompt.h"
enum {
	None = 0,
	Black = 30,
	Red = 31,
	Green = 32,
	Brown = 33,
	Blue = 34,
	Magenta = 35,
	Cyan = 36,
	White = 37
};
#endif
