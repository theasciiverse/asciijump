/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define CMDLINE_C
#include "cmdline.h"

static char 
*usage =
" usage: "PACKAGE" [ARG]\n"
" --help or -h          show this message\n"
" --version or -V       show version info message\n"
" --matrix or -m\n"
" --matrix_full or -mf  use one of matrix mode\n",

*version =
"  "PACKAGE" v "VERSION"\n"
"  [ copyright (c) grzegorz moskal,  eldevarth@nemerle.org ]\n\n" 
"  this program comes with absolutely no warranty; for details\n"
"  please see the file 'COPYING' supplied with the source code.\n"
"  this is free software, and you are welcome to redistribute it\n"
"  under certain conditions; again, see 'COPYING' for details.\n"
"  this program is released under the gnu general public license v2.\n";


int parse_cmdline(int argc, char **argv)
{
	int i = 1;
	
	for (; i < argc; i++) 
		switch(optionid(argv[i])) {
		case Help:
			printf(usage);
			return 0;
		case Version:
			printf(version);
			return 0;
		case Matrix_full:
			mx_mode = MATRIX_FULL;
			break;
		case Matrix:
			mx_mode = MATRIX;
			break;
		default:
			printf("option ``%s'' not recognized :(", argv[i]);
			printf("try %s --help", argv[0]);
			return 0;
		}
		
	return 1;
}

static int opt(char *pattern, char *src1, char *src2)
{
	if (strcmp(pattern, src2) == 0 || strcmp(pattern, src1) == 0) 
		return 1;

	return 0;
}

#define OPTION(a,b) opt(arg, a, b)

static int optionid(char *arg)
{
	if (OPTION("--help", "-h")) 
		return Help;
	else if (OPTION("--version", "-V")) 
		return Version;
	else if (OPTION("--matrix_full", "-mf"))
		return Matrix_full;
	else if (OPTION("--matrix", "-m"))
		return Matrix;
	return 0;
}
