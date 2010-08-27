/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define CLIENT_C
#include "client.h"


char *cl_name = "foo";
char *cl_server = "127.0.0.1";
short int cl_port = 2051;

int cl_socket;
int cl_debug = 0;
static struct sockaddr_in cl_addr;
static struct s_skier *cl_sk_head;
static struct s_skier *cl_sk_tail;
static struct s_skier *cl_sk_current;
static struct s_skier **cl_sk_array = NULL;
static struct s_skier *cl_skier;

static int cl_sk_counter;
static int cl_sk_index;

static struct s_hill *cl_hill;

c_state cl_state = C_read;

void cl_show(void)
{
	cl_scr_show();
	cl_winstatus_show();
	state = A_client;
}

void cl_init()
{
	// state C_init is used to avoid black screen.
	
	cl_winstatus_cleanup();
	cl_winstatus_show();
	
	cl_display(MSG, "init network, please wait ..");
	cl_display(MSG, "port = %d, name = %s", cl_port, cl_name);
	
	cl_state = C_init;
	state = A_client;
	
	cl_sk_head = cl_sk_tail = NULL;	
	sk_head = sk_tail = NULL;
	sk_array = NULL;
	sk_counter = cl_sk_counter = 0;

	rs_actions(cl_show, mn_scr_show);
}

void cl_display(char *prompt, char *fmt, ...)
{
	va_list ap;
	char *s;
	int d;
	int arg = 0;

	if (strcmp(prompt, DEBUG) == 0 && cl_debug == 0)
		return;

	va_start(ap, fmt);
	cl_winstatus_puts(prompt);

	for (; *fmt; fmt++) 
		switch (*fmt) {
		case '%': 
			arg = 1;
			continue;
		case 's':        
			if (arg) {
				s = va_arg(ap, char *);
				cl_winstatus_puts(s);
				arg = 0;
			} else
				cl_winstatus_putc(*fmt);	
			break;
		case 'd':           
			if (arg) {
				d = va_arg(ap, int);
				cl_winstatus_putd(d);
				arg = 0;
			} else 
				cl_winstatus_putc(*fmt);
			break;
		case '\n':
			cl_winstatus_puts("\n");
			cl_winstatus_puts(prompt);
			break;
		case '|':
			if (*(fmt+1) == '\0')
				goto display_end;
		default:
			cl_winstatus_putc(*fmt);
		}
	cl_winstatus_puts("\n");
	display_end:
	va_end(ap);
}

static void cl_net(void)
{	
        struct hostent *hostinfo;
	
	// this state is set when something wrong.
	state = A_menu;
	
	if ((cl_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		cl_display(ERR, "socket() faild - cannot continue"); 
		return;
	}
	
	cl_display(DEBUG, "socket created successful");
	

        cl_addr.sin_family = AF_INET;
        cl_addr.sin_port = htons(cl_port);
	
        if ((hostinfo = gethostbyname(cl_server)) == NULL) {
		cl_display(ERR, "unknown host - cannot continue"); 
		return;
        }
	
        cl_addr.sin_addr = *(struct in_addr *) hostinfo->h_addr;
	
	if (connect(cl_socket, (struct sockaddr *)&cl_addr, sizeof(cl_addr)) == -1) {
		cl_display(ERR, "socket() faild - cannot continue"); 
		return;
	}
	
	cl_display(MSG, "connection established, waiting for the hill.");

	write(cl_socket, cl_name, strlen(cl_name)+1);
	
	state = A_client;
	cl_state = C_read;
}

void cl_read_id(char *line)
{
	struct s_skier *s;
	
	cl_sk_index = strtol(line, NULL, 10);

	for (s = cl_sk_head; s; SWITCH(s))
		if (s->id == cl_sk_index)
			break;
			
	
	cl_display(DEBUG, "index = %d", cl_sk_index);
	
	if ((cl_sk_current = s) == NULL)
		cl_display(DEBUG, "unknown index");
}

static void cl_read_hill(char *line)
{
	int len = strtol(line, &line, 10);
	char *name = line;
	
	xfree(cl_hill);	// TODO hl_free should be written ! 
	cl_hill = hl_init(len, name);
	
	cl_display(MSG, "new hill recived: name = %s, length = %d", cl_hill->name, cl_hill->len);
		
	cl_state = C_play;	
	sk_flush(cl_skier, cl_hill);
}

static void cl_read_results(char *line)
{
	char buf[10];
	int pt = strtol(line, &line, 10);
 	cl_sk_current->last_points = pt;
	cl_sk_current->points += pt;
	
	cl_display(DEBUG, "(s) result for %s: %d ", cl_sk_current->name, pt);
}

static void cl_read(void)
{
	char *member, *line, *msg, buf[10];
	
	member = line = xread(cl_socket);

	if (line == NULL || *line == '\0')
		return;
	
//	cl_display(DEBUG, "\"%s\"| ", line);

	switch (*line++) {
	case Id:
		cl_read_id(line);
		break;
	case Hill:
		cl_read_hill(line);
		break;
	case Results:
		cl_read_results(line);
		break;
	case Everyone:
                sk_array_sort();
		rs_scr_reinit(cl_sk_array, cl_sk_counter, cl_hill);
		state = A_menu;
		cl_state = C_read;
		break;
	case You:
		if (sk_head == 0)
			cl_display(MSG, "users:");
		sk_add(sk_init(cl_hill, skin, line));
		cl_skier = sk_tail;
		cl_skier->id = cl_sk_index;
		cl_display(MSG, "you = %s", line);
		break;
	case User:
		if (sk_head == 0)
			cl_display(MSG, "users:");
		sk_add(sk_init(cl_hill, skin, line));
		sk_tail->id = cl_sk_index;
		cl_display(MSG, "user = %s", line);
		break;
	case Msg:
		cl_display(LOGO, "%s", line);
		break;
	case Kick:
		cl_display(MSG, "you were kicked");
		state = A_menu;
		break;
	case Bye:
		rs_scr_reinit(cl_sk_array, cl_sk_counter, NULL);
		break;
		
	default:
		cl_display(DEBUG, "unrecognized string: %s", line-1);
		break;
	}
}

void cl_play(int key)
{
	if (cl_sk_array == NULL) {
		struct s_skier *s;
		
		sk_counter = sk_tail->id + 1;
		
		cl_sk_tail = sk_tail;
		cl_sk_head = sk_head;
		cl_sk_counter = sk_counter;
		
		cl_sk_array = xmalloc(sizeof(struct s_skier *) * cl_sk_counter);

		for (s = cl_sk_head; s; SWITCH(s))
			cl_sk_array[s->id] = s;
	
		sk_array = cl_sk_array;

		sk_flush(cl_skier, cl_hill);
	}
	
	if (sk_service(cl_skier, key)) {
		play(cl_skier);
		return;
	}
	
	cl_skier->points -= cl_skier->last_points;
	cl_state = C_result;
	sl_cls();
}

static void cl_result()
{
	char buf[20];
	sprintf(buf, "%c%d\n", Results, (int)cl_skier->last_points);
	cl_display(DEBUG, "sending results = %d", (int)cl_skier->last_points);
	write(cl_socket, buf, strlen(buf));
	cl_state = C_read;
}


void client(int key)
{
	switch (cl_state) {
	case C_init:
		cl_state = C_net;
		menu(key);
		break;
	case C_net:
		cl_net();
		menu(key);
		break;
	case C_read:
		menu(key);
		cl_read();
		break;
	case C_play:
		cl_play(key);
		break;
	case C_result:
		menu(key);
		cl_result();	
		break;
	}
}
