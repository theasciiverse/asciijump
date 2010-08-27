#ifndef AS_PARSE_H
#define AS_PARSE_H
struct keyword_action {
	char *caption;
	void (*action)(void);

};
#ifdef AS_PARSE_C
#include <string.h>	// strcmp()
#include "as_display.h"	// as_display()
#include "as_hill.h"	// as_hill_parse()
#include "as_getline.h"	// as_argv
#include "as_let.h"	// as_let_parse()
#include "as_user.h"	// as_user_parse()
#include "aserver.h"	// as_state
static void as_show_help(void);
#endif
int as_run_keyword_action(struct keyword_action ka[]);
void as_server_start(void);
int as_parse(void);
#endif
