/* :: asciijump (server), gnu gpl v 2
   :: copyright (c) grzegorz moskal, eldevarth@nemerle.org */
   
#define AS_LET_C
#include "as_let.h"

struct var *as_let_head, *as_let_tail;

static void as_let_add_var(struct var *v)
{
	if (as_let_head == NULL)
		as_let_head = as_let_tail = v;
	else {
		as_let_tail->next = v;
		SWITCH(as_let_tail);
	}
}

void as_let_add_int(char *name, int *dest, int lower, int upper, char *help)
{
	struct var *v = XALLOC(var);
	struct int_var *iv = XALLOC(int_var);
	
	iv->dest = dest;
	iv->lower = lower;
	iv->upper = upper;

	v->variable = (void *)iv;
	v->name = strdup(name);
	v->type = VINT;
	v->help = strdup(help);

	v->next = NULL;
	as_let_add_var(v);	
}

void as_let_add_str(char *name, char **dest, char *help)
{
	struct var *v = XALLOC(var);
	struct str_var *sv = XALLOC(str_var);
	
	sv->dest = dest;

	v->variable = (void *)sv;
	v->name = strdup(name);
	v->type = VSTRING;
	v->help = strdup(help);

	v->next = NULL;
	as_let_add_var(v);	
}

static void as_let_init(void)
{
	as_let_add_int("colors", &as_allow_colored_prompt, 0, 1,
		"define if display colors");
	as_let_add_int("port", (int *)&as_port, 1000, 20000, 
		"define port to listen for this program");
	as_let_add_int("players", &as_limit, 2, 20,
		"define max player limit");
	as_let_add_str("name", &as_name,
		"define this server name (displayed in users)");
	as_let_add_str("hilldir", &as_hilldir,
		"define hill directory (used when typed: hill read)");
	as_let_add_int("verbose", &as_debug, 0, 1,
		"define if display diagnostic output");
}

static struct keyword_action as_let_keywords[] = {
	{ "show", as_let_show },
	{ "help", as_let_help },
	{ NULL, NULL}
};

void as_let_parse(void)
{
	if (as_let_head == NULL)
		as_let_init();
		
	if (as_argv[1] == 0) 
		as_let_show();
		
	else if (as_run_keyword_action(as_let_keywords) == 0)
		as_let_set();
}

static void as_let_set()
{
	char *word = as_argv[1];
	struct var *v = as_let_find_var(word);
	
	if (v == NULL)
		as_display(ERR, "there is no varible called: %s", word);
	else if ((word = as_argv[2]) == NULL) 
		as_let_show_var(v, List);
	else
		as_let_set_var(v, word);
}

static void as_let_set_int(struct int_var *iv, char *s)
{
	int nv = strtol(s, NULL, 10);
	if (nv < iv->lower || nv > iv->upper)
		as_display(ERR, "%s: value out of bound", s);
	else {
		*(iv->dest) = nv;
		as_display(MSG, "new value: %d", *(iv->dest));
	}
}

static void as_let_set_str(struct str_var *sv, char *s)
{
	*(sv->dest) = strdup(s);
	as_display(MSG, "new value: %s", *(sv->dest));
}

static void as_let_set_var(struct var *v, char *s)
{
	switch (v->type) {
	case VINT:
		as_let_set_int((struct int_var *)v->variable, s);
		break;
	case VSTRING:
	case VFILE:	// tmp, pwd not taken!
		as_let_set_str((struct str_var *)v->variable, s);
		break;
	}
}

static struct var *as_let_find_var(char *name)
{
	struct var *v = as_let_head;
	
	for (; v; SWITCH(v))
		if (strcmp(v->name, name) == 0)
			break;
	return v;
}

static void as_let_show_var(struct var *v, int type)
{
	if (v == NULL)
		return;
		
	else if (v->type == VINT) {
		struct int_var* iv = (struct int_var *)v->variable;
		if (type == List)
			as_display(MSG, "%s = %d", v->name,  *(iv->dest));
		else
			as_display(MSG, "%s <int [%d - %d]> - %s", v->name, 
				iv->lower, iv->upper, v->help);
	} else {
		struct str_var* sv = (struct str_var *)v->variable;
		if (type == List)
			as_display(MSG, "%s = %s", v->name, *(sv->dest));
		else 
			as_display(MSG, "%s <%s> - %s", v->name,
				(v->type == VSTRING) ? "string" : "file", v->help);
	}
}

static void as_let_show_all(int state)
{
	struct var *v = as_let_head;
	
	for (v = as_let_head; v; SWITCH(v)) 
		as_let_show_var(v, state);
}

static void as_let_show(void) 
{
	char *word = as_argv[1];
	struct var *v;
	
	if (word == NULL || (word = as_argv[2]) == NULL || strcmp(word, "*") == 0)
		as_let_show_all(List);
	else if ((v = as_let_find_var(word)))
		as_let_show_var(v, List);	
	else
		as_display(ERR, "there is no variable called: %s", word);
}


static char *as_let_help_text = 
"let [option]\n\n"
"let show - display all variables\n"
"let show ${name} - display given variable\n"
"let help - show this help\n"
"let help ${name} - remove given let\n"
"let ${name} foo - set value of $name to foo\n\n"
"current variables are:";

static void as_let_help()
{
	char *word = as_argv[2];
	struct var *v;
	
	if (word == NULL) {
		as_display(MSG, as_let_help_text);
		as_let_show_all(Help);
	} else if ((v = as_let_find_var(word)))
		as_let_show_var(v, Help);	
	else
		as_display(ERR, "there is no variable called: %s", word);
}
