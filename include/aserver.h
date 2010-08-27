#ifndef ASERVER_H
#define ASERVER_H

typedef enum asciijump_server_state { 
AS_finish,	
// adding clients or switching to Command state if stdin is not empty
AS_listen, 	
// read and parse stdin
AS_command,	
// reading data from clients or switching to Command state if stdin .. 
AS_read, 		
// sending player list to the clients, seting all inital data to start play
AS_play, 		
// prepare jump, - send hill length and name to the clients, switch hill.
AS_prepare,	
} s_state;

typedef enum s_as_player_status {
P_del,	// when xread(player->socket) return NULL, (player disconnet)
P_done, // when player sent result
P_read	// after sending hill to the players; 
} p_status;

struct s_as_player;
struct s_as_player {
	p_status status;
	int socket;
	char *hostname;
	char *name;
	int points, result;	// player`s points
	int id;				// player`s id
	struct s_as_player *next, *prev;
};

struct s_as_players {
	struct s_as_player *head;
	struct s_as_player *tail;
	int counter;
};

#ifdef ASERVER_C
#include <signal.h>		// signal()
#include <string.h>		// strdup()
#include <stdio.h>		// printf()
#include <netinet/in.h>		// ntohs()
#include <sys/types.h>		// bind()
#include <sys/socket.h>		// socket()
#include <arpa/inet.h>		// inet_ntoa()
#include <stdlib.h>		// exit()
#include <unistd.h>		// alarm()
#include <errno.h>		// errno
#include <sys/poll.h>		// poll()
#include "xfnc.h"		// XALLOC() macro
#include "as_getline.h"		// as_getline()
#include "as_parse.h"		// as_parse()
#include "as_display.h"		// as_errexit()
#include "as_hill.h"		// as_hill_current pointer.

static struct s_as_player *as_ff_players(int n);
static void as_send_hill(void);
static void as_prepare_players(void);
static int as_invite_players(void);
static s_state as_read(void);
static s_state as_prepare_jump(void);
static int as_command(void);
static int as_loop(void);
static int as_every_results_read(void);
static void as_del_players(void);
static void as_send2players(n_type t, char *buf);

#else

extern struct s_as_players *as_players;
extern short int as_port;
extern int as_limit;
extern s_state as_state;
extern s_state as_work;
extern char *as_name;

#endif
#include "xfnc.h"
void as_set_state(s_state);
void as_listen(void);
int as_create_socket(void);
void as_send(struct s_as_player *p, n_type type, char *buf);
void as_del_player(struct s_as_player *p);

#endif
