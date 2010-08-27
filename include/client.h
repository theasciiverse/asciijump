#ifndef CLIENT_H
#define CLIENT_H
#ifdef  CLIENT_C
#include <sys/types.h>	// connect()
#include <netdb.h>	// gethostbyname()
#include <netinet/in.h>	// htons()
#include <sys/socket.h>	// connect()
#include <stdlib.h>	// exit()
#include <stdio.h>	// perror()
#include <string.h>	// strlen()
#include <unistd.h>	// write()
#include <errno.h>	// ;)
#include "main.h"	// game_show()
#include "scrmain.h"	// scrmain_show()
#include "scrclient.h"	// ;) 
#include "xfnc.h"	// xread()
#include "skier.h"	// sk_head
#include "hill.h"	// hl_init()
#include "prompt.h"	// MSG define
#include <stdarg.h>	// va_ ... macros
typedef enum asciijump_client_state {
C_read,
C_result,
C_play,
C_init, 
C_net
} c_state;
void cl_display(char *prompt, char *fmt, ...);
static void cl_result(void);
static void cl_read(void);
#else 
extern char *cl_name;
extern char *cl_server;
extern short int cl_port;
#endif
void cl_show(void);
void cl_init(void);
void client(int key);
#endif
