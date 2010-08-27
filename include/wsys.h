#ifndef WSYS_H
#define WSYS_H
#define OBJ_DOWN (-2)
#define OBJ_UNTICKABLE (-1)

typedef enum wsys_widget_type {
Wsys_screen,
Wsys_window,
Wsys_inputline,
Wsys_textbox,
Wsys_menu,
Wsys_menuobj
} wsys_type;

struct widget;
struct widget {
	int hidden, selectable;
	int wheel;
	int x, y, w, h;
	struct widget *mother, *kids, *last, *next, *current;
	void (*draw)(struct widget *);
	int (*key)(struct widget *, int);
	void *data;
	wsys_type type;
};
#define NEXT_WIDGET (302)
#define PREV_WIDGET (-302)
#ifdef WSYS_C
#include <string.h>
#include "slangwrap.h"
#include "xfnc.h"

// struct menuobj and struct inputline =>
struct line {
	void (*action)(void);
	char *caption;
	int length, place;
	int tick;
	int key;
};

// ptr->data = xalloc(sizeof(char *)); :P
struct window {
	char *title;
	int len;
};

// ptr->data = xalloc(sizeof(int)); :P
struct menu {
	int tickable;
};

/* wsys.c */
static void list_add(struct widget *mother, struct widget *child);
static struct widget *widget_init(int x, int y, int w, int h);
static void window_draw(struct widget *w);
static void widget_draw(struct widget *w);
static void inputline_draw(struct widget *i);
static void menuobj_draw(struct widget *mo);
static void textbox_draw(struct widget *t);
static int screen_key(struct widget *screen, int key);
static int window_key(struct widget *win, int key);
static int menu_key(struct widget *menu, int key);
static int inputline_key(struct widget *i, int key);
#endif

void list_switch(struct widget *w, int how);
struct widget *screen_add(void);
struct widget *window_add(struct widget *scr, char *title, 
	int x, int y, int w, int h);
struct widget *inputline_add(struct widget *win, int x, int y, int w, void (*fnc)(void));
struct widget *menu_add(struct widget *win, int x, int y, int w, int h, int);
struct widget *menuobj_add(struct widget *menu, char *title,int,  void (*fnc)(void), int tick);
struct widget *textbox_add(struct widget *win, char *text);

void textbox_insert(struct widget *tb, char *text);
void textbox_cleanup(struct widget *tb, char *text);

char *menu_selected_caption(struct widget *m);
struct widget *label_add(struct widget *win, int x, int y, int w, char *text);
void inputline_fill(struct widget *i, char *text);
char *inputline_caption(struct widget *i);

int menu_selected_number(struct widget *m);
void menu_select(struct widget*w, int n);
struct widget *menu_ticked(struct widget *m);

void screen_hideall(struct widget *win);
void menu_tick(struct widget*mo);

struct widget *line_free(struct widget *l);
struct widget* menu_free(struct widget *m);
struct widget* window_free(struct widget *win);
#endif
