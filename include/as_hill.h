#ifndef AS_HILL_H
#define AS_HILL_H
struct as_hill;
struct as_hill {	
	char *name;
	int length;
	struct as_hill *next, *prev;
};
#ifdef AS_HILL_C
#include <string.h>
#include <stdlib.h>
#include "as_display.h"
#include "as_parse.h"
#include "as_getline.h"
#include "xfnc.h"
static void as_hill_cleanup(void);
static void as_hill_delete(struct as_hill *hill);
static void as_hill_add(struct as_hill *hill);
static void as_hill_show(void);
static void as_hill_parse_delete(void);
static void as_hill_parse_add(void);
static void as_hill_help(void);
static void as_hill_service_adding(void);
static void as_hill_parse_read(void);
static void as_hill_read(void);
static struct as_hill *as_hill_init(char *name, int len);
#else
extern struct as_hill *as_hill_head;
extern struct as_hill *as_hill_tail;
extern struct as_hill *as_hill_current;
extern int as_hill_counter;
extern char *as_hilldir;
#endif
void as_hill_parse(void);
#endif
