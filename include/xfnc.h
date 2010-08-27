#ifndef XFNC_H
#define XFNC_H

#define SWITCH(j) (j=j->next)
#define PREV(j) (j=j->prev)
#define XALLOC(sname) xmalloc(sizeof(struct sname))

typedef enum asciijump_network_type {
Bye	 = 'b',
Everyone = 'e',
Hill 	 = 'h',
Id	 = 'i',
Kick	 = 'k',
Msg	 = 'm',
Results  = 'r',
User 	 = 'u',
You	 = 'y'
} n_type;


#include <stdio.h>
#include <dirent.h>

#ifdef XFNC_C
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
static int hill_readconf(char *filename);
#else 
extern char *hill_name;
extern int hill_length;
#endif
char *xgetcwd(void);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void *xfree(void *);
char *xgetline(FILE *fd);
FILE *xfopen(char *name, char *mode);
DIR *xopendir(char *name);
void xchdir(char *name);
char *strglue(const char *s1, const char *s2, const char *s3);
char *strdglue(const char *p1, const char *p2);
char *strwide(char *s);
int d(int hm);
void rand_init(void);
char *xread(int fd);
char *n_type_description(n_type t);
int hill_readdir_extend(char *dirname, int *counter, void (*fnc)(void));
#endif
