/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define FRAME_C
#include "frame.h"

struct frame* fr_init(char *filename)
{
	FILE *fd = xfopen(filename,"r");
	struct frame *f = xmalloc(sizeof(struct frame));
	int h = 4;
	
	f->h = 0;
	f->w = 0;
	f->next = f->prev = 0;
	f->filename = strdup(filename);
	f->caption = xmalloc(sizeof(char*)*h);

	do {
		if (h == f->h) {
			h *= 2;
			f->caption = xrealloc(f->caption, h*sizeof(char *));
		}
		
		f->caption[f->h] = xgetline(fd);
		
	} while (f->caption[f->h++]);

	f->h--;
	
	fclose(fd);

	return f;
}

void fr_draw(struct frame *f, int x, int y, char *hot, int color, int color2)
{
	int i = 0, ix;
	char *line;
	
	sl_color(color);

	for (; f->caption[i]; i++) {
		for (ix = 0, line = f->caption[i]; *line; ix++, line++) {
		
			if (*line == ' ')
				continue;
				
			sl_goto(x + ix, y+i);
			
			if (strchr(hot,*line)) 
				sl_color(color2);
			else
				sl_color(color);
				
			sl_putch(*line);
		}
	}
}
