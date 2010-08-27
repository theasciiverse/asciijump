#ifndef AS_LET_H
#define AS_LET_H
#ifdef AS_LET_C
#include <stdlib.h>	// strtol
#include <string.h>	// strcmp()
#include "as_display.h"	// as_display() as_allow_colored_prompt
#include "xfnc.h"	// XALLOC macro.
#include "aserver.h"	// as_port, as_limit, as_name
#include "as_parse.h"	// struct keyword_action ..
#include "as_getline.h"	// as_argv, as_argc
#include "as_hill.h"	// as_hilldir


typedef enum var_type { VINT, VSTRING, VFILE } vtype;
enum { Help, List };

struct str_var {
	char **dest;
};

struct int_var {
	int *dest;
	int lower, upper;
};

struct var;
struct var {
	void *variable;
	char *name;
	char *help;
	vtype type;
	struct var *next;
};

static void as_let_init(void);
static void as_let_set(void);
static void as_let_set_int(struct int_var *iv, char *s);
static void as_let_set_str(struct str_var *sv, char *s);
static void as_let_set_var(struct var *v, char *s);
static struct var *as_let_find_var(char *name);
static void as_let_show_var(struct var *v, int type);
static void as_let_show_all(int state);
static void as_let_show(void);
static void as_let_help(void);
static void as_let_add_var(struct var *v);
#endif
void as_let_add_int(char *name, int *dest, int lower, int upper, char *help);
void as_let_add_str(char *name, char **dest, char *help);
void as_let_parse(void);
#endif
