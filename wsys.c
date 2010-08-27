/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, eldevarth@nemerle.org */

#define WSYS_C
#include "wsys.h"

static void list_add(struct widget *mother, struct widget *child)
{
	if (mother->kids == NULL)
		mother->last = mother->kids = child;
	else {
		mother->last->next = child;
		mother->last = child;
	}

	if (mother->current && mother->current->selectable == 0)
		SWITCH(mother->current);
	else if (mother->current == NULL)
		mother->current = child;

	child->mother = mother;
}

void list_switch(struct widget *w, int how)
{
	struct widget *jmp;
	if (how == NEXT_WIDGET) {
		if (w->current->next) {
			for (jmp = w->current->next; jmp; SWITCH(jmp))
				if (jmp->selectable && jmp->hidden == 0) {
					w->current = jmp;
					break;
				}
		} else if (w->wheel && w->current != w->kids)
			for (jmp = w->kids; jmp->next != w->current; SWITCH(jmp))
				if (jmp->selectable && jmp->hidden == 0) {
					w->current = jmp;
					break;
				}
	} else if (how == PREV_WIDGET) {
		if (w->current != w->kids) {
			struct widget *arg = w->current;
			for (; arg != w->kids; arg = jmp) {
				for (jmp = w->kids; jmp->next != arg; SWITCH(jmp));
				if (jmp->selectable && jmp->hidden == 0) {
					w->current = jmp;
					break;
				}
			}
		} else if (w->wheel)
			if (w->last->selectable && w->last->hidden == 0)
				w->current = w->last;
	}
	
}

static struct widget *widget_init(int x, int y, int w, int h)
{
	struct widget *wee = XALLOC(widget);
	wee->x = x;
	wee->y = y;
	wee->w = w;
	wee->h = h;
	wee->kids = wee->next = wee->current = wee->mother = NULL;
	wee->data = NULL;
	wee->key = NULL;
	wee->draw = widget_draw;
	wee->selectable = 1;
	return wee;
}

struct widget *screen_add()
{
	struct widget *scr = widget_init(0, 0, sl_screen_width, sl_screen_height);
	scr->key = screen_key;
	scr->type = Wsys_screen;
	return scr;
}

struct widget *window_add(struct widget *scr, char *title, int x, int y, int w, int h)
{
	struct widget *win = widget_init(x+scr->x, y+scr->y, w, h);
	
	struct window *twin = XALLOC(window);
	twin->title = strdup(title);
	twin->len = sl_strlen_alt(title);
	win->data = twin;
	
	win->wheel = 1;
	
	win->draw = window_draw;
	win->key = window_key;
	
	list_add(scr, win);
	win->type = Wsys_window;
	textbox_add(win, "");
	
	return win;
}

struct widget *inputline_add(struct widget *win, int x, 
	int y, int w, void (*fnc)(void))
{
	struct widget *i = widget_init(x+win->x, y+win->y, w, 1);
	i->data = XALLOC(line);
	i->draw = inputline_draw;
	i->key = inputline_key;
	((struct line *)i->data)->action = fnc;
	list_add(win, i);
	i->type = Wsys_inputline;
	return i;
}

struct widget *menu_add(struct widget *win, int x, int y, int w, int h, int tickable)
{
	struct widget *m = widget_init(x+win->x, y+win->y, w, h);
	
	struct menu *tm = XALLOC(menu);
	tm->tickable = tickable;
	m->data = tm;
	
	list_add(win, m);
	m->type = Wsys_menu;
	m->key = menu_key;
	return m;
}

struct widget *menuobj_add(struct widget *menu, char *title, 
	int key, void (*fnc)(void), int tick) 
{
	struct widget *mo ;
	int y;
	
	struct line *tmo = XALLOC(line);
	tmo->caption = strdup(title);
	tmo->place = strlen(title);
	tmo->action = fnc;
	
	if (menu->last)
		y = menu->last->y +1;
	else
		y = menu->y;
	if (tick <= OBJ_DOWN) {
		tick -= OBJ_DOWN;
		y++;
	}
	tmo->tick = tick;
	tmo->key = key;
	
	mo = widget_init(menu->x, y, menu->w, 1);
	mo->data = tmo;
	mo->type = Wsys_menuobj;

	mo->draw = menuobj_draw;
	mo->key = NULL;

	list_add(menu, mo);
	
	return mo;
}

struct widget *textbox_add(struct widget *win, char *text)
{
	struct widget *tb = widget_init(win->x+1, win->y+1, win->w-2, win->h-2);
	if (text)
		tb->data = strdup(text);
	
	list_add(win, tb);
	tb->type = Wsys_textbox;
	tb->draw = textbox_draw;
	tb->key = NULL;
	tb->selectable = 0;
	return tb;
}

void textbox_cleanup(struct widget *tb, char *text)
{
	char *s = (char *)tb->data;
	xfree(tb->data);
	tb->data = strdup("");
}

void textbox_insert(struct widget *tb, char *text)
{
	char *s = (char *)tb->data;
	s = strglue(s, text, "");
	xfree(tb->data);
	tb->data = (void *)s;
}

struct widget *label_add(struct widget *win, int x, int y, int w, char *text)
{
	struct widget *i = inputline_add(win, x, y, w, 0);
	i->selectable = 0;
	((struct line *)i->data)->caption = strdup(text);
	return i;
}


static void widget_draw(struct widget *w)
{
	struct widget *jmp = w->kids;
	for (; jmp; SWITCH(jmp))
		if (jmp->hidden == 0)
			jmp->draw(jmp);
	if (w->current && w->current->hidden == 0)
		w->current->draw(w->current);
}

static void window_draw(struct widget *win)
{
	struct window *twin = ((struct window *)win->data);
	struct widget *jmp = win->kids;
	
	sl_color(White);
	sl_frame(win->x, win->y, win->w, win->h);
	
	sl_goto(win->x +1, win->y);
	sl_putch(' ');
	sl_addstr_alt(twin->title, twin->len, Green, Lgreen);
	sl_putch(' ');

	for (; jmp; SWITCH(jmp))
		if (jmp->hidden == 0)
			jmp->draw(jmp);

	win->current->draw(win->current);
}

static void inputline_draw(struct widget *i)
{
	struct line *il = ((struct line *)i->data);
	sl_color(White);
	if (il->caption) {
		sl_goto(i->x, i->y);
		sl_addstr(il->caption);
		sl_putch(' ');
	}
	sl_goto(i->x + il->place, i->y);
}

static void textbox_draw(struct widget *t)
{
	int x = t->x;
	int y = t->y;
	char *jmp = (char *)t->data;

	if (jmp == NULL)
		return;
	
	sl_color(White);
	for (; y < t->y + t->h; y++) {
		sl_goto(t->x, y);
		for (x = t->x; x < t->x + t->w; x++) {
			if (*jmp == '\n') {
				jmp++;
				break;
			} else if (*jmp)
				sl_putch(*jmp++);
			else
				break;
				
		}
		for (; x < t->x + t->w; x++)
			sl_putch(' ');
	}
}

static void menuobj_draw(struct widget *mo)
{
	struct line * tmo = ((struct line * )mo->data);
	int i;
	int color = Passive , color2 = Red;
	
	if (mo->mother->current == mo 
		&& mo->mother->mother->current == mo->mother) {
		color = Active;
		color2 = Active2;
	}
	
	sl_color(color);
	sl_goto(mo->x, mo->y);
	for (i = 0; i < mo->w-4; i++) {
		if (i < tmo->place) {
			if (tmo->caption[i] == tmo->key) {
				sl_color(color2);
				sl_putch(tmo->caption[i]);
				sl_color(color);
			} else
				sl_putch(tmo->caption[i]);
			
		} else
			sl_putch(' ');
	}
	
	sl_addstr((tmo->tick == 1)? " ::" : "   ");
	if (mo->mother->current == mo)
		sl_putch('<');
	else 
		sl_putch(' ');
}

static int screen_key(struct widget *screen, int key)
{
	if (key == 0)
		return 0;
	switch (key) {
	case sl_key_f(1):
		list_switch(screen, NEXT_WIDGET);
		return 1;
	case sl_key_f(2):
		list_switch(screen, PREV_WIDGET);
		return 1;
	default:
		if (screen->current->key(screen->current, key))
			return 1;
	}
	return 0;
	
}

static int window_key(struct widget *win, int key)
{
	switch (key) {
	case '\t' :
		if (win->current->type == Wsys_inputline)
			win->current->key(win->current, '\n');
		else
			list_switch(win, NEXT_WIDGET);
		return 1;
	default:
		if (win->current && win->current->key
			&& win->current->key(win->current, key))
			return 1;
	}
	return 0;
}

static int menu_key(struct widget *menu, int key)
{
	struct line *li = ((struct line*)menu->current->data);
	struct widget *mo;
	switch (key) {
	case sl_key_pgup:
		if (menu->kids->selectable && menu->kids->hidden == 0)
			menu->current = menu->kids;
		return 1;
	case sl_key_pgdn:
		if (menu->last->selectable && menu->last->hidden == 0)
			menu->current = menu->last;
		return 1;
	case sl_key_up:
		list_switch(menu, PREV_WIDGET);
		return 1;
	case sl_key_down:
		list_switch(menu, NEXT_WIDGET);
		return 1;
	case '\n':
		if (li->action) {
			li->action();
			return 1;
		}
	case ' ':
		if (((struct menu *)menu->data)->tickable && li->tick != -1) {
			li->tick = (li->tick == 0);
			list_switch(menu, NEXT_WIDGET);
			return 1;
		}
		break;
	default:
		for (mo = menu->kids; mo; SWITCH(mo)) 
			if (((struct line *)mo->data)->key == key) {
				menu->current = mo;
				return 1;
			}
			
	}
	return 0;
}

static int inputline_key(struct widget *i, int key)
{
	struct line *il = ((struct line *)i->data);
	switch (key) {
	case '\b':
		if (il->place > 0) {
			il->place--;
			il->caption[il->place] = '\0';
		}
		return 1;
	case '\n':
		if (((struct line *)i->data)->action)
			((struct line *)i->data)->action();
		list_switch(i->mother, NEXT_WIDGET);
		
		return 1;
	case sl_key_right:
		if ((unsigned int)il->place < strlen(il->caption)) 
			il->place++;
		return 1;
	case sl_key_left:
		if (il->place > 0)
			il->place--;
		return 1;
	default :
		if (il->place > il->length -2) {
			il->length = il->length*2 + 3;
			il->caption = xrealloc(il->caption, il->length * sizeof(char));
		} 
//		if (il->place < strlen(il->caption)) {
//			char *tmp = strdup(il->caption+il->place);
//			
//		} else {
			il->caption[il->place++] = key;
			il->caption[il->place] = '\0';
//		}
	}
	return 1;
}

char *menu_selected_caption(struct widget *m)
{
	return ((struct line *)m->current->data)->caption;
}

int menu_selected_number(struct widget *m)
{
	struct widget *jmp = m->kids;
	int i = 0;
	for (; jmp; SWITCH(jmp), i++)
		if (jmp == m->current)
			return i;
	return -1;
}

void menu_select(struct widget*w, int n)
{
	w->current = w->kids;
	while (n-- > 0)
		list_switch(w, NEXT_WIDGET);
}

struct widget *menu_ticked(struct widget *m)
{
	for (; m; SWITCH(m))
		if (((struct line*)m->data)->tick)
			break;
	return m;
}

void menu_tick(struct widget*mo)
{
	((struct line*)mo->data)->tick = 1;
}

void inputline_fill(struct widget *i, char *text)
{
	struct line *il = ((struct line *)i->data);
	xfree(il->caption);
	il->caption = strdup(text);
	il->place = strlen(text);
	il->length = il->place;
}

char *inputline_caption(struct widget *i)
{
	return ((struct line *)i->data)->caption;
}

void screen_hideall(struct widget *win)
{
	struct widget *wj = win->kids;
	for (; wj; SWITCH(wj))
		wj->hidden = 1;
	win->current->hidden = 0;
	sl_cls();
}

struct widget *line_free(struct widget *l)
{
	if (l == 0)
		return l;
	xfree(((struct line*)l->data)->caption);
	xfree(l->data);
	return xfree(l);
}

struct widget* menu_free(struct widget *m)
{
	struct widget *w, *w2;
	
	if (m == NULL) 
		return m;
		
	for (w = m->kids; w;) {
		w2 = w->next;
		line_free(w);
		w = w2;
	}
	
	xfree(m->data);
	return xfree(m);
}

struct widget* window_free(struct widget *win)
{
	if (win == NULL)
		return win;
		
	if (win->mother->kids == win)
		win->mother->kids = win->mother->current = win->mother->last = NULL;
	
	xfree(((struct window *)win->data)->title);
	xfree(win->data);
	return xfree(win);
}
