/* :: asciijump (server), gnu gpl v 2
   :: copyright (c) grzegorz moskal, eldevarth@nemerle.org */
   
#define ASERVER_C
#include "aserver.h"

struct s_as_players *as_players;
uint16_t as_port = 2051;
int as_limit = 2;
s_state as_state = AS_command;
s_state as_work = AS_command;
char *as_name = "classic asciijump";

static int as_socket;

char *as_state_array[] = { 
"AS_bye",	
"AS_listen", 	
"AS_command",	
"AS_read", 		
"AS_play", 		
"AS_prepare",	
};


void as_send_users()
{
	struct s_as_player *p;
	char buf[4];
	
	// first whole user list is sending to the new client
	for (p = as_players->head; p; SWITCH(p)) {

		sprintf(buf, "%d", p->id);
		
		if (p == as_players->tail) {
			as_send(p, Id, buf);
			as_send(p, You, p->name);
			break;
		}
		
		as_send(as_players->tail, Id, buf);
		as_send(as_players->tail, User, p->name);
	}
	
	as_del_players();
	
	// if this client is not first, he`s name is sending to others
	if (as_players->head != as_players->tail) {
		sprintf(buf, "%d", as_players->tail->id);
		for (p = as_players->head; p != as_players->tail; SWITCH(p)) {

			as_send(p, Id, buf);
			as_send(p, User, as_players->tail->name);
		}
	}
		
	as_del_players();
}

void as_send_results()
{
	struct s_as_player *p, *p2;
	char buf[6];
	
	for (p2 = as_players->head; p2; SWITCH(p2)) {
		for (p = as_players->head; p; SWITCH(p)) {
			sprintf(buf, "%d", p->id);
			as_send(p2, Id, buf);
			sprintf(buf, "%d", p->result);
			as_send(p2, Results, buf);
			// sending id
		}
		as_send(p2, Everyone, "");
	}

	as_del_players();
}

void as_add_player()
{
	struct s_as_player *player = XALLOC(s_as_player);
	struct sockaddr_in addr;
	socklen_t size = sizeof(addr);

	if ((player->socket = accept(as_socket, (struct sockaddr *)&addr, &size)) == -1) 
		as_eerr("accept", 5);
	
	player->hostname = strdup(inet_ntoa(addr.sin_addr));
	player->status = P_read;
	
	if ((player->name = xread(player->socket)) == NULL) {
		as_display(MSG, "broken connection from: %s\n", player->hostname);
		xfree(player->hostname);
		xfree(player);
		return;
	}
		
	if (as_players->head == NULL)
		as_players->tail = as_players->head = player;
	else {
		player->prev = as_players->tail;
		as_players->tail->next = player;
		SWITCH(as_players->tail);
		player->id = player->prev->id + 1;
	}

	
	as_display(DEBUG, "adding : %s", player->name);
	as_display(MSG, "new connection from host:%s, port:%d, name:%s", 
		player->hostname, ntohs(addr.sin_port), player->name);
		
	as_players->counter++;
	as_send(player, Msg, as_name);
	as_send_users();
}

void as_del_player(struct s_as_player *p)
{
	if (p->next) {
		if (p->prev) {
			p->prev->next = p->next;
			p->next->prev = p->prev;
		} else {
			p->next->prev = NULL;
			SWITCH(as_players->head);
		}
	} else {
		if (p->prev) {
			p->prev->next = NULL;
			PREV(as_players->tail);
		} else {
			as_players->head = as_players->tail = NULL;
		}
	}

	as_players->counter--;
		
	as_display(DEBUG, "deleting : %s", p->name);
	xfree(p->hostname);
	xfree(p->name);
	xfree(p);
}

static struct s_as_player *as_ff_players(int n)	
{
	struct s_as_player *p = as_players->head;
	
	while (n-- > 0)
		SWITCH(p);
		
	return p;
}

void as_send(struct s_as_player *p, n_type type, char *buf)
{
	int len = strlen(buf) + 3;
	char *s = malloc(sizeof(char)*len);
	sprintf(s, "%c%s\n", type, buf);
	as_display(DEBUG, "sending to %s [ %s: %s ]", p->name, n_type_description(type), buf);
	
	if (write(p->socket, s, len) < len) {
		as_display(DEBUG, "cannot write to: %s@%s, he`s deleting ;)", p->name, p->hostname);
		p->status = P_del;
	}
}

static void as_send2players(n_type t, char *buf)
{
	struct s_as_player *p = as_players->head;
	
	for (; p; SWITCH(p)) 
		as_send(p, t, buf);
		
	as_del_players();
}

static void as_send_hill(void)
{
	char *buf = xmalloc(sizeof(char) * (strlen(as_hill_current->name) + 10));
	
	sprintf(buf, "%d%s", as_hill_current->length, as_hill_current->name);
	as_send2players(Hill, buf);
	
	xfree(buf);

	SWITCH(as_hill_current);
}

static void as_prepare_players(void)
{
	struct s_as_player *p = as_players->head;
	
	for (; p; SWITCH(p))
		p->status = P_read;
}

int as_create_socket(void)
{
	struct sockaddr_in name;

	if ((as_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
		as_eerr("socket", 3);

	name.sin_family = AF_INET;
	name.sin_port = htons(as_port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(as_socket, (struct sockaddr *) &name, sizeof(name)) == -1) {
		as_display(ERR, "bind problem:");
		printf("\r");
		perror("bind faild");
		printf("\n");
		return 0;
	}
	return 1;
}

void as_listen(void)
{
	if (listen(as_socket, as_limit) == -1) 
		as_eerr("listen", 4);
	as_display(MSG, "server name: %s", as_name);
	as_display(MSG,	"listen port: %d", as_port);
	as_display(MSG, "users limit: %d", as_limit);
}

static int as_invite_players(void)
{
	struct pollfd ufds [] = { {0, POLLIN, 0}, {as_socket, POLLIN, 0} };
	

	poll(ufds, 2, 1000);

	if (ufds[0].revents & POLLIN) 
		return 1;

	else if (ufds[1].revents & POLLIN) {
		if (as_players->counter < as_limit) 
			as_add_player();
		else 
			return 0x0;
	}

	return as_invite_players();
}

static int as_every_results_read(void)
{
	struct s_as_player *p = as_players->head;
	
	for (; p; SWITCH(p))
		if (p->status == P_read) 
			return 0;
	return 1;
}

static void as_del_players(void)
{
	struct s_as_player *p;
	for (p = as_players->head; p;) {
		if (p->status == P_del) {
			as_del_player(p);
			p = as_players->head;
		} else
			SWITCH(p);
	}
}

static s_state as_read()
{
	s_state ret = as_state;
	const int len = as_players->counter +1;
	struct pollfd as_ufds[len];
	int i = 0;
	struct s_as_player *p = as_players->head;
	char *line;
	
	for (i = 0; i < len-1; SWITCH(p), i++) {
		as_ufds[i].fd = p->socket;
		as_ufds[i].events = POLLIN;
		as_ufds[i].revents = 0;
	}
	
	as_ufds[i].events = POLLIN;
	as_ufds[i].fd = 0;
	as_ufds[i].revents = 0;
	
	if (poll(as_ufds, as_players->counter+1, 1000) == -1)
		as_eerr("poll", 8);
		
	if (as_ufds[as_players->counter].revents & POLLIN)  
		return AS_command;

		
	for (i = 0; i < len-1; i++) {
			
		if (!(as_ufds[i].revents & POLLIN))
			continue;

		p = as_ff_players(i);
		line = xread(p->socket);
			
		if (line == NULL) {
			p->status = P_del;
			continue;
		} else if (*line != Results)
			continue;
			
		p->result = strtol(line + 1, NULL, 10);
		as_display(MSG, "%s@%s jump result: %d", p->name, p->hostname, p->result);
		xfree(line);
		
		p->status = P_done;	
	}

	as_del_players();
	if (as_every_results_read() == 0) 
		return as_read();
	else {
		as_send_results();
		ret = AS_prepare;
	}

	return ret;	
}

static s_state as_prepare_jump()
{
	if (as_hill_current == NULL) 
		return AS_finish;
	
	as_prepare_players();
	as_send_hill();

	return AS_read;
}

void as_set_state(s_state s)
{
	as_state = s;
//	as_display(DEBUG, as_state_array[s]);
}


static int as_command(void)
{
	switch (as_getline()) {
	case T_EOF:
		return 0;
	case T_SEMI:		
		if (as_parse() == 0)
			return 0;
		as_loop();
		break;
	case T_ERROR:
		as_display(ERR, "syntax error");
	case T_NL:
		if (as_parse() == 0)
			return 0;
		if (as_state == AS_command)
			as_set_state(as_work);
		as_prompt();
	}
	return 1;
}

void as_finish()
{
	struct s_as_player *p = as_players->head;
	
	as_send2players(Bye, "");
	
	// closing sockets
	for (; p; SWITCH(p))
		close(p->socket);

	as_display(LOGO, "thanks for testing.");	
	exit(0);
}

static int as_loop()
{
	switch (as_state) {
	case AS_listen:
		if (as_invite_players() == 0) 
			as_display(ERR, "to much players, try change limit"
				" (let limit 20)");
			
		as_set_state(AS_command);
		break;
		
	case AS_command:
		if (as_command() == 0)
			as_set_state(AS_finish);
		break;
		
	case AS_read:
		as_set_state(as_read());
		break;
		
	case AS_play:
		as_display(MSG, "game started successful");
		as_set_state(AS_prepare);
		break;
		
	case AS_prepare:
		as_set_state(as_prepare_jump());
		break;

	case AS_finish:	
		as_finish();
		return 0;
	}
	
	return 1;
}

void as_init(void)
{
	signal(SIGINT, as_finish);

	as_players = XALLOC(s_as_players);
	
	as_display(LOGO, "U can`t B dead all the time");
	as_display(MSG, "welcome to asciijump experimental server.");

	rand_init();
}

int main()
{
	as_init();

	for (;;) 
		if (as_loop() == 0x0)
			break;

	return 0;
}
