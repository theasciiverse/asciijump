/* :: asciijump (server), gnu gpl v 2
   :: copyright (c) grzegorz moskal, eldevarth@nemerle.org */
   
#define AS_USER_C
#include "as_user.h"

static void as_user_delete(struct s_as_player *p)
{
	char *text = as_argv[as_current+1];
	
	if (text == NULL)
		text = "karma police";
		
	as_send(p, Msg, text);
	as_send(p, Kick, "");
	as_del_player(p);
}

static void as_user_cleanup()
{
	struct s_as_player *p;
	
	for (p = as_players->head; p; SWITCH(p))
		as_user_delete(p);
	
}

static struct keyword_action as_user_keywords[] = {
	{ "show", as_user_show },
	{ "kick", as_user_parse_kick },
	{ "let", NULL },
	{ "help", as_user_help },
	{ NULL, NULL}
};

void as_user_parse(void)
{
	if (as_argv[as_current] == NULL) 
		as_user_help();

	else if (as_run_keyword_action(as_user_keywords) == 0)
		as_display(ERR, "bad magic word, try: user help");
}

static void as_user_display(struct s_as_player *p)
{
	as_display(MSG, "name:%s, host:%s, id:%d, points:%d", 
		p->name, p->hostname, p->id, p->points);
}


static void as_user_show()
{
	struct s_as_player *p = as_players->head;
	char *word = as_argv[as_current];

	if (as_players->head == NULL) 
		as_display(MSG, "sorry .. there is no players, try wait for them");
		
	else if (word == NULL || strcmp(word, "*"))
		for (p = as_players->head; p; SWITCH(p))
			as_user_display(p);
			
	else 
		for (p = as_players->head; p; SWITCH(p)) {
			if (strstr(p->name, word) || strstr(p->hostname, word) || 
				atoi(word) == p->points || atoi(word) == p->id)
				as_user_display(p);
		}
}

static void as_user_parse_kick()
{
	char *word = as_argv[as_current];
	struct s_as_player *p;
	int n = as_players->counter;

	if (word == NULL) {
		as_display(ERR, "no input user, try type user help");
		return;
	}
	
	if (*word == '*')
		as_user_cleanup();
		
	else if (word[strlen(word)-1] == '*') {
		int len = strlen(word) - 2;
		again:
		for (p = as_players->head; p; SWITCH(p))
			if (strncmp(p->name, word, len) == 0) {
				as_user_delete(p);
				goto again;
			}
	} else 
		for (p = as_players->head; p; SWITCH(p))
			if (strcmp(p->name, word) == 0 || atoi(word) == p->id)
				as_user_delete(p);
				
	n -= as_players->counter;
	
	if (n != 0)
		as_display(MSG, "%d user%s deleted", n, (n > 1) ? "s" : "");
	else
		as_display(ERR, "pattern not found: %s", word);
}

static char *as_player_help_text = 
"user [option] [suboption]\n\n"
"user show - display all users\n"
"user kick ${name} - remove given user\n"
"     kick foo  - remove user named foo\n"
"     kick *    - remove all users\n"
"     kick foo* - remove all users begining by foo\n"
"user help - show this help screen";

void as_user_help()
{
	as_display(MSG, as_player_help_text);		
}
