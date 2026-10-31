#ifndef HILL_H
#define HILL_H
struct s_skier;
struct s_hill;
struct s_hill {
	// pointer to next hill
	struct s_hill *next;
	// array of int, every single element is hill`s height in current index.
	int *caption;
	// that array of ints, is used to draw diffirent kinds of floor
	int *fill_no;
	// caption length
	int len;
	// point K
	int k;
	// hill record
	double record;
	// 2 variables used to better loaclize skier
	int pns, pnz;
	// leapsill
	int leapsill;
	// hill name
	char *name;
	// hill owenr`s name (owner is a man who beat the hill record)
	char *owner;
};

#ifdef  HILL_C
#include <math.h>
#include <stdio.h>	// fopen()
#include <ctype.h>	
#include <time.h>	
#include <string.h>	// strstr()
#include <stdlib.h>
#include <unistd.h>	
#include <sys/stat.h>	// mkdir()
#include "xfnc.h"	// XALLOC macro	
#include "slangwrap.h"	// sl_color()
#include "skier.h"	// struct skier
static int ix2(int x, double p);
static void hl_fill(struct s_hill *h);
static void hl_add(struct s_hill *h);
static int hl_readconf(char *filename);
static void hl_service_adding(void);
#else
extern struct s_hill *hl_head, *hl_tail;
extern int hl_counter;
#endif
struct s_hill *hl_init(int len, char *name);
struct s_hill *hl_find(char *pattern, struct s_hill *head);
int hl_readdir(char *dirname);
void hl_draw(struct s_skier *j);
int hl_record_write(struct s_hill *h);
void hl_record_read(struct s_hill *h);
#endif
