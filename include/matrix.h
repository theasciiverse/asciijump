#ifndef MATRIX_H
#define MATRIX_H

#define MATRIX_FULL (103)
#define MATRIX (102)
#define SNOW (101)

#ifdef MATRIX_C
#include <string.h>
#include "slangwrap.h"
#include "xfnc.h"

struct column;
struct column {
	struct column *next;
	int last_y, height;
	int async, async_pattern;
	short shine, draw, stop, cut;
	char *caption;
};

static void col_flush(struct column *c);
static void col_check_grow(struct column *c);
static void col_service(struct column *c);
static void col_draw(struct column *c, int x);
#else
extern int mx_mode;
#endif
void mx_init(int x, int y, int w, int h);
void mx_draw(void);
void mx_move(int n);
void mx_service(void);
void mx_change_mode(int c_normal, int c_shine, int mode);
void mx_free(void);
#endif
