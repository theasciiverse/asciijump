/* :: asciijump (server), gnu gpl v 2
   :: copyright (c) grzegorz moskal, eldevarth@nemerle.org */
   
#define AS_HILL_C
#include "as_hill.h"

struct as_hill *as_hill_head;
struct as_hill *as_hill_tail;
struct as_hill *as_hill_current;
int as_hill_counter;
char *as_hilldir = DATADIR"/hills";

static void as_hill_cleanup(void)
{
	struct as_hill *jumper, *tmp;

	for (jumper = as_hill_head; jumper;) {
		tmp = jumper;
		SWITCH(jumper);
		xfree(tmp->name);
		xfree(tmp);
	}

	as_hill_head = as_hill_tail = as_hill_current = NULL;
	as_hill_counter = 0;
}

static void as_hill_delete(struct as_hill *hill)
{
		
	if (hill->next) {
		if (hill->prev) {
			hill->prev->next = hill->next;
			hill->next->prev = hill->prev;
		} else {
			hill->next->prev = NULL;
			SWITCH(as_hill_head);
		}
	} else {
		if (hill->prev) {
			hill->prev->next = NULL;
			PREV(as_hill_tail);
		} else {
			as_hill_head = as_hill_tail = NULL;
		}
	}
		
	as_hill_counter--;
	as_display(DEBUG, "hill deleted, %s %d", hill->name, hill->length);
}

static void as_hill_add(struct as_hill *hill)
{
	if (as_hill_head == NULL)
		as_hill_head = as_hill_tail = as_hill_current = hill;
	else {
		hill->prev = as_hill_tail;
		as_hill_tail->next = hill;
		SWITCH(as_hill_tail);
	}
	
	as_hill_counter++;
	as_display(DEBUG, "hill added, %s %d", hill->name, hill->length);
}

static struct keyword_action as_hill_keywords[] = {
	{ "show", as_hill_show },
	{ "delete", as_hill_parse_delete },
	{ "del", as_hill_parse_delete },
	{ "add", as_hill_parse_add },
	{ "read", as_hill_parse_read },
	{ "help", as_hill_help },
	{ NULL, NULL}
};

void as_hill_parse()
{
	if (as_argv[as_current] == NULL) 
		as_hill_help();

	else if (as_run_keyword_action(as_hill_keywords) == 0)
		as_display(ERR, "bad magic word, try: hill help");
}

static void as_hill_show()
{
	struct as_hill *hill = as_hill_head;
	char *line = "name = %s, length = %d", *buf;
	int len = 0;

	if (as_hill_head == NULL) {
		as_display(MSG, "sorry .. there is no hill, try: hill add");
		return;
	}
	
	for (; hill; SWITCH(hill)) 
		if (strlen(hill->name) > (size_t)len)
			len = strlen(hill->name);

	buf = xmalloc(sizeof(char) * (strlen(line) + len + 12));
	
	for (hill = as_hill_head; hill; SWITCH(hill)) {
		sprintf(buf, line, hill->name, hill->length);
		as_display(MSG, buf);
	}

	xfree(buf);

}


static void as_hill_service_adding(void)
{
	as_hill_add(as_hill_init(hill_name, hill_length));
}

static void as_hill_read(void)
{
	if (hill_readdir_extend(as_hilldir, &as_hill_counter, as_hill_service_adding) == -1)
		as_display(ERR, "no such directory, try type: let hilldir /foo/hilldir");
		
	else if (as_hill_counter == 0)
		as_display(MSG, "empty dir.");
}

static void as_hill_read_random(void)
{
	struct as_hill *hill, *h2, *head = as_hill_tail;
	
	as_hill_read();

	if (head == NULL)
		head = as_hill_head;
	

	for (hill = head; hill; ) {
		if (d(2) == 2) {
			h2 = hill;
			SWITCH(hill);
			as_hill_delete(h2);
		} else
			SWITCH(hill);
	}

	as_hill_current = as_hill_head;
}

static void as_hill_parse_read(void)
{
	char *word = as_argv[as_current];
	struct as_hill * hill;
	int n = as_hill_counter;

	if (word == NULL || strcmp(word, "*") == 0) 
		as_hill_read();
		
	else if (strcmp(word, "random") == 0 || strcmp(word, "r") == 0) 
		as_hill_read_random();
		
	else
		as_display(ERR, "unknown keyword: %s", word);
}

static void as_hill_parse_delete()
{
	char *word = as_argv[as_current];
	struct as_hill * hill;
	int n = as_hill_counter;

	if (word == NULL) {
		as_display(ERR, "no input hill, try type hill help");
		return;
	}
	
	if (*word == '*')
		as_hill_cleanup();
		
	else if (word[strlen(word)-1] == '*') {
		int len = strlen(word) - 2;
		again:
		for (hill = as_hill_head; hill; SWITCH(hill))
			if (strncmp(hill->name, word, len) == 0) {
				as_hill_delete(hill);
				goto again;
			}
	} else 
		for (hill = as_hill_head; hill; SWITCH(hill))
			if (strcmp(hill->name, word) == 0) 
				as_hill_delete(hill);
				
	n -= as_hill_counter;
	
	if (n)
		as_display(MSG, "%d hill%s deleted", n, (n > 1) ? "s" : "");
	else
		as_display(ERR, "pattern not found: %s", word);
}

static struct as_hill *as_hill_init(char *name, int len)
{
	struct as_hill * hill = XALLOC(as_hill);
	
	hill->name = strdup(name);
	hill->length = len;
	
	if (len < 0)
		hill->length *= -1;
		
	return hill;
}

static void as_hill_parse_add()
{
	char *name = NULL, *line = NULL;
	int len;
	
	if (as_argc < 3) {
		as_display(MSG, "please write hill`s name: ");
		name = xgetline(stdin);
	} else
		name = strdup(as_argv[2]);
		
	if (as_argc < 4) {
		as_display(MSG, "please write hill`s length: ");
		line = xgetline(stdin);
		len = strtol(line, NULL, 10);
	} else 
		len = strtol(as_argv[3], NULL, 10);
	
	if (len > 20 && len < 300) {
		as_display(MSG, "hill added successful, name: %s, length: %d", name, len);
		as_hill_add(as_hill_init(name, len));
	} else 
		as_display(MSG, "length %d - value out of bound <20, 300>", len);
		
	xfree(line);
	xfree(name);
}

static char *as_hill_help_text = 
"hill [option] [suboption]\n\n"
"hill show - display all hills\n"
"hill delete ${name} - remove given hill\n"
"     delete foo  - remove hill named foo\n"
"     delete *    - remove all hills\n"
"     delete foo* - remove all hills begining by foo\n"
"hill add ${name} ${length} - add given hill\n"
"hill read - it read every files from hilldir directory\n"
"hill random - it read randomize files from hilldir directory\n"
"[hill dir could be change be typing: let hilldir /foo/bar]"
"hill help - show this help screen";

static void as_hill_help()
{
	as_display(MSG, as_hill_help_text);		
}
