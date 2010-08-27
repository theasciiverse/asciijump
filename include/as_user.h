#ifndef AS_USER_H
#define AS_USER_H
#ifdef AS_USER_C
#include <string.h>
#include <stdlib.h>
#include "as_display.h"
#include "as_parse.h"
#include "as_getline.h"
#include "aserver.h"	// as_players
#include "xfnc.h"
static void as_user_delete(struct s_as_player *p);
static void as_user_cleanup(void);
static void as_user_show(void);
static void as_user_display(struct s_as_player *p);
static void as_user_parse_kick(void);
#endif
void as_user_parse(void);
void as_user_help(void);
#endif
