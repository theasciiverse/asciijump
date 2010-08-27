/* :: asciijump (server), gnu gpl v 2
   :: copyright (c) grzegorz moskal, eldevarth@nemerle.org */
   
#define AS_GETLINE_C
#include "as_getline.h"

char **as_argv;
int as_argc;
int as_current;

#define INC 10

static void as_c_put(char **v, int c)
{
	static int len, i;
	static char *word = 0;

	if (v == NULL) {
		word = xfree(word);
		len = 0;
		i = -1;
		return;
	}
	
	if (++i == len) {	
		len = i*2+INC;	
		word = xrealloc(word, sizeof(char) * (len+1));
		*v = word;
	}
	word[i] = c;
}
	
static lex as_getword(char **v)
{
	enum { NORMAL, STRING, WORD } state = NORMAL;
	int c;

	as_c_put(NULL, 0);	
	*v = NULL;
	
	while ((c = getchar()) != EOF) {
		switch (state) {
		case NORMAL:
			switch (c) {
			case ';':
				return T_SEMI;
			case '\n':
				return T_NL;
			case '\t':
			case ' ':
				continue;
			case '"':
				state = STRING;
				continue;
			default :
				state = WORD;
				as_c_put(v, c);
				continue;	
			}
		case STRING:
			switch (c) {
			case '"':
				as_c_put(v, '\0');
				return T_WORD;
			case '\\':
				as_c_put(v, getchar());
				continue;
			default:
				as_c_put(v, c);
				continue;
			}
			
		case WORD:
			switch (c) {
			case ';':
			case '\n':
			case '\t':
			case ' ':
				ungetc(c, stdin);
				as_c_put(v, '\0');
				return T_WORD;
			default :
				as_c_put(v, c);
				continue;
			}
		}
	}
	return T_EOF;
}

int as_getline(void)
{
	
	char *word;
	int len = 0;
	lex l;


	if (as_argc > 0) {
		while (--as_argc >= 0)
			xfree(as_argv[as_argc]);
		xfree(as_argv);
	}

	as_argv = NULL;
	as_argc = 0;
	as_current = 0;
	
	for (;;) {
		if (as_argc == len) {
			len = as_argc*2 + INC;
			as_argv = xrealloc(as_argv, sizeof(char*)*(len+1));
		}
		
		switch ((l = as_getword(&word))) {
		case T_WORD:
			as_argv[as_argc++] = strdup(word);
			continue;
			
		case T_SEMI:
		case T_NL:
			as_argv[as_argc] = NULL;
		
			if (as_argc == 0 && l != T_NL) 
				return T_ERROR;
				
		default:
			return l;
		
		}
	}

	return 0xdead;	// never returned.
}
