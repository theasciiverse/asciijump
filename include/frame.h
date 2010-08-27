#ifndef FRAME_H
#define FRAME_H
struct frame;
#ifdef FRAME_C

#include <string.h>
#include "xfnc.h"
#include "slangwrap.h"
#endif

struct frame {
	struct frame *next, *prev;
	char *filename;
	char **caption;
	int w, h;
	int state;
};
struct frame *fr_init(char *filename);
void fr_draw(struct frame *f, int x, int y, char *hot, int color, int color2);
#endif
