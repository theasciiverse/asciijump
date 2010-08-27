/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define SCRCLIENT_C
#include "scrclient.h"

static struct widget* cl_scr;
static struct widget* cl_win_data;
static struct widget* cl_line_name;
static struct widget* cl_line_server;
static struct widget* cl_line_port;
static struct widget* cl_win_status;
static struct widget* cl_tbox_status;


void cl_winstatus_putd(int digit)
{
	char buf[10];
	sprintf(buf, "%d", digit);
	textbox_insert(cl_tbox_status, buf);
}

void cl_winstatus_putc(char c)
{
	char buf[2];
	buf[0] = c;
	buf[1] = '\0';
	textbox_insert(cl_tbox_status, buf);
}

void cl_winstatus_puts(char *text)
{
	textbox_insert(cl_tbox_status, text);
}

void cl_winstatus_show(void)
{
	cl_win_status->hidden = 0;
	cl_scr->current = cl_win_status;
	sl_cls();
}

void cl_winstatus_cleanup(void)
{
	textbox_cleanup(cl_tbox_status, "");
}

void cl_scr_show(void)
{
	if (cl_scr == NULL)
		cl_scr_init();
		
	cl_scr->current = cl_win_data;
	cl_win_status->hidden = 1;
	screen = cl_scr;
	state = A_menu;

	sl_cls();
}

void cl_set_name(void)
{
	//TODO xfree(cl_name)
	cl_name = strdup(inputline_caption(cl_line_name));
}

void cl_set_server(void)
{
	cl_server = strdup(inputline_caption(cl_line_server));
	
}

void cl_set_port(void)
{
	char *s = inputline_caption(cl_line_port);
	cl_port = strtol(s, NULL, 10);
}

static void cl_winstatus_init(void)
{
	int w = WIDTH/10 * 8+2;
	cl_win_status = window_add(cl_scr, "_status _window", 2, 2, w, HEIGHT-2);
	menuobj_add(menu_add(cl_win_status, 1, HEIGHT-4, w-2, 2, 0), 
		"abort <<(",'(', cl_scr_show, 0);
	cl_win_status->hidden = 1;
	cl_tbox_status = textbox_add(cl_win_status, " ");
}

void cl_scr_init(void)
{
	#define CL_LINE_Y 4
	char buf[6];
	int w = WIDTH/10 * 8+2;
	int h = HEIGHT/10 * 8;
	
	cl_scr = screen_add();
	cl_win_data = window_add(cl_scr, PACKAGE " client v " VERSION, 1, 1, w, h);
		
	menuobj_add(menu_add(cl_win_data, 1, 1, w-2, 1, 0), 
		"connect", 'c', cl_init, 0);

	label_add(cl_win_data, 1, CL_LINE_Y, 10, "name: ");
	cl_line_name = inputline_add(cl_win_data, 11, CL_LINE_Y, w-12, cl_set_name);
	inputline_fill(cl_line_name, cl_name);
	
	label_add(cl_win_data, 1, CL_LINE_Y+1, 10, "server: ");
	cl_line_server = inputline_add(cl_win_data, 11, CL_LINE_Y+1, w-12, cl_set_server);
	inputline_fill(cl_line_server, cl_server);
	
	label_add(cl_win_data, 1, CL_LINE_Y+2, 10, "port: ");
	cl_line_port = inputline_add(cl_win_data, 11, CL_LINE_Y+2, w-12, cl_set_port);
	sprintf(buf, "%hd", cl_port);
	inputline_fill(cl_line_port, buf);
	
	menuobj_add(menu_add(cl_win_data, 1, h-2 , w-2, 1, 0), 	"<<(", '(', mn_scr_show, 0);
	
	cl_winstatus_init();	
}
