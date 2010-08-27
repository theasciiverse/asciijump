/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define MATRIX_C
#include "matrix.h"


#define START (53)
static struct column *first, *last;

static int mx_width;
static int mx_height;
static int mx_x;
static int mx_y;
static int mx_color_normal;
static int mx_color_shine;
int mx_mode = SNOW;

static void col_flush(struct column *c)
{
	int i = 0;

	for (; i < mx_height; i++)
		c->caption[i] = ' ';
	c->caption[i] = '\0';
	
	c->last_y = 0;
	c->height = 0;
	c->shine = (d(5) == 5);
	c->stop = 1;
	c->draw = 1;
	c->cut = 0;
	c->async = 0;
	c->async_pattern = d(2)+1;
}

void mx_init(int x, int y, int w, int h)
{
	struct column *c;
	int i = 0;

	mx_x = x;
	mx_y = y;
	mx_width = w;
	mx_height = h;
	
	mx_color_normal = (mx_mode < MATRIX_FULL) ? Black : Grey;
	mx_color_shine = (mx_mode < MATRIX_FULL) ? Grey : White;

	
	for (; i < w; i++) {
		c = xmalloc(sizeof(struct column));
		c->height = h;
		c->caption = xmalloc(sizeof(char)*(c->height+1));
		c->next = 0;
		col_flush(c);
		if (first) {
			last->next = c;
			last = last->next;
		} else
			first = last = c;
	}
	for (i = 40; i; i--)
		mx_service();
}

void mx_free()
{
	struct column *tmp, *c = first;
	while (c) {
		tmp = c;
		SWITCH(c);
		xfree(tmp->caption);
		xfree(tmp);
	}
	first = last = NULL;
}

static void col_check_grow(struct column *c)
{
	int p = -1;
	int i = mx_height/2;

	for (; i < mx_height; i+=4)
		if (c->height == i)
			p = 3*i;
	
	if (d(100) <= p)  
		c->cut = 1;
}

static void col_service(struct column *c)
{
	if (c->stop) {
		c->stop = d(START)-1;
		return;
	}

	if (c->async > 0)
		c->async--;
	else {
		c->async = d(c->async_pattern);
		if (c->draw) {
			col_check_grow(c);
			c->caption[c->last_y] = d(93) + 33;
			c->last_y++;
			c->height++;
		} 
		if (c->cut)
			c->caption[c->last_y - c->height--] = ' ';
	}

	if (c->last_y >= mx_height-1) {
		c->draw = 0;
		c->cut = 1;
	}
	
	if (c->height == 0 && c->cut == 1) 
		col_flush(c);
}

void mx_service(void)
{
	struct column *c = first;

	for (c = first; c; c = c->next)
		col_service(c);

	for (c = first; c; c = c->next)
		col_service(c);
}

static void col_draw(struct column *c, int x)
{
	char *tab = ".";
	int len = strlen(tab);
	int h = 0;
	
	sl_color(mx_color_normal);
	
	for (; h < mx_height; h++) {
		sl_goto(x, h+mx_y);
		
		// if it is not the last
		if (h+1 != c->last_y) {
			sl_putch(c->caption[h]);
			continue;
		}
		// elsewere
		sl_color(mx_color_shine);
		if (mx_mode != SNOW)
			sl_putch(c->caption[h]);
		else 
			sl_putch(tab[d(len)-1]);
		sl_color(mx_color_normal);
	}
}

void mx_draw(void)
{
	struct column *c = first;
	int x = mx_x;
	
	for (; c; c = c->next) 
		col_draw(c, x++);
}

void mx_move(int n)
{
	struct column *tmp;
	while (n--) {
		tmp = first;
		first = first->next;
		last->next = tmp;
		last = last->next;
		last->next = 0;
	}
}

void mx_change_mode(int c_normal, int c_shine, int mode)
{
	mx_mode = mode;
	mx_color_normal = c_normal;
	mx_color_shine = c_shine;
}
