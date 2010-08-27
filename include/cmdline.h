#ifndef CMDLINE_H
#define CMDLINE_H
#ifdef CMDLINE_C
#include <stdlib.h>
#include <string.h>
#include "xfnc.h"
#include "matrix.h"
enum {
	Help = 1,
	Version,
	Matrix,
	Matrix_full
};

static int opt(char *pattern, char *src1, char *src2);
static int optionid(char *arg);
#endif
int parse_cmdline(int argc, char **argv);
#endif
