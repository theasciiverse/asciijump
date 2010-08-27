/* :: ascii-jump project
   :: $Id: skin.c,v 1.1.1.1 2003/02/28 10:05:04 loth Exp $
   
   :: (C)opyright 2003  grzegorz moskal, g.moskal@opengruop.org
   :: license gnu gpl v 2						*/

#define SKIN_C
#include "skin.h"

static struct frame* skin_init(char *dirname)
{
	struct frame *first = NULL, *last = NULL;
	char no[9], *filename;
	FILE *fd;

	int i = 1;

	for (;i < 100; i++) {
		sprintf(no, "%s%d.frame", (i < 10) ? "0" : "", i);
		filename = strdglue(dirname, no);
		if ((fd = fopen(filename, "r")) == NULL)
			break;
		fclose(fd);
			
		if (last) {
			last->next = fr_init(filename);
			last->next->prev = last;
			last = last->next;
		} else
			first = last = fr_init(filename);
		xfree(filename);
	}
	
	xfree(filename);
	return first;
}

static char *dirnames[] = {
	"01start", 
	"02descent", 
	"03ride", 
	"04jump", 
	"05fly", 
	"06telemark",
	"07descent2",
	"08ride2",
	"09stop",
	"10crash",
	NULL
};

struct frame* skin_make(char *prefix)
{
	struct frame *all_frames = NULL, *last, *tmp;
	int i = 0;
	char *dirname;
	for (; dirnames[i]; i++) {
		dirname = strdglue(prefix, dirnames[i]);
		if ((tmp = skin_init(dirname)) == NULL)
			continue;
		if (all_frames == NULL) 
			all_frames = tmp;
		else {
			for (last = all_frames; last->next; SWITCH(last));
			last->next = tmp;
			tmp->prev = last;
		}
		for (; tmp; SWITCH(tmp))
			tmp->state = i;
		
		xfree(dirname);
	}
	return all_frames;
}
