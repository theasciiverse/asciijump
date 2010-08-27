#ifndef AS_GETLINE_H
#define AS_GETLINE_H
typedef enum {
	T_ERROR = 0,	
	T_WORD, 	// arg
	T_SEMI,		// ;
	T_NL,		// '\n' ;)
	T_EOF		// end-of-file or ^D => EOT
} lex;
#ifdef AS_GETLINE_C
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "xfnc.h"

static void as_c_put(char **v, int c);
static lex as_getword(char **);
#else
extern char **as_argv;
extern int as_current;
extern int as_argc;
#endif
int as_getline(void);
#endif
