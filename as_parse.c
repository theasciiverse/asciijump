/* :: asciijump (server), gnu gpl v 2
   :: copyright (c) grzegorz moskal, eldevarth@nemerle.org */
   
#define AS_PARSE_C
#include "as_parse.h"

char *as_help_text =
PACKAGE " v " VERSION "\n"
"usage: <command> [options]\n\n"
"let      -  set given variable\n"
"hill     -  add, del or show given hill\n"
"invite   -  invite players\n"
"play     -  start new game\n"
"user     -  change user data\n"
"? | help -  print this message\n"
"foo help -  show help screen for command foo.\n\n"
"instead of <command> you can type only first letter.\n"
"eg: \"h a foo 100\" is same to \"hill add foo 100\".\n\n"
"type: bye to exit program";

static void as_show_help()
{
	as_display(MSG, as_help_text);
}

void as_server_play()
{
	if (as_work != AS_listen) {
		as_display(ERR, "before start, you should invite players,"
			"try type: invite");
		return;
	} 
	
	if (as_hill_counter <= 0) {
		as_display(ERR, "cannot start game, cause there is no hill, "
			"try type: hill add");
	} 
	
	if (as_players->counter <= 0) {
		as_display(ERR, "cannot start game, cause there is no player, "
			"try wait.");
	}
	
	if (as_work == AS_read) {
		as_display(ERR, "game has been already started");
		
	} else if (as_hill_counter > 0 && as_players->counter > 0) {
		as_set_state(AS_play);
		as_work = AS_read;
	}
}

void as_server_listen()
{
	if (as_work != AS_command)
		as_display(ERR, "cannot listen again. sorry..");
	
	else {
		if (as_create_socket() == 0) {
			as_display(MSG, "try to change port: let port 2222");
			return;
		}
		as_listen();
		as_work = AS_listen;
		as_set_state(AS_listen);
	}
}

struct keyword_action as_keywords[] = {
	{ "invite", as_server_listen },
	{ "hill", as_hill_parse },
	{ "play", as_server_play },
	{ "let", as_let_parse },
	{ "help", as_show_help },
	{ "user", as_user_parse },
	{ "?", as_show_help },
	{ NULL, NULL }
};

int as_run_keyword_action(struct keyword_action ka[])
{
	int i = 0;
	char *word = as_argv[as_current];
	
	for (; ka[i].caption != NULL; i++)
		if (strcmp(word, ka[i].caption) == 0 || 
		   (strlen(word) == 1 && ka[i].caption[0] == word[0])) {
			as_current++;
			ka[i].action();
			return 1;
		}

	return 0;
}

int as_parse(void)
{
	if (as_argc == 0)
		return 3;

	if (strcmp(as_argv[as_current], "bye") == 0 || 
		strcmp(as_argv[as_current], "quit") == 0)
		return 0;
	
	if (as_run_keyword_action(as_keywords) == 0) {
		as_display(ERR, "bad magic word: %s, try type: help",
			as_argv[as_current]);
	}
	
	return 1;
}

