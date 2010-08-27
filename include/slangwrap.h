#ifndef SLANGWRAP_H
#define SLANGWRAP_H

#ifdef SLANG_C
#define C_BLACK "black"
#define C_GRAY "gray"
#define C_RED "red" 
#define C_LIGHTRED "brightred"
#define C_GREEN "green"
#define C_LIGHTGREEN "brightgreen"
#define C_BROWN "brown"
#define C_YELLOW "yellow"
#define C_BLUE "blue"
#define C_LIGHTBLUE "brightblue"
#define C_MAGENTA "magenta"
#define C_LIGHTMAGENTA "brightmagenta"
#define C_CYAN "cyan"
#define C_LIGHTCYAN "brightcyan"
#define C_LIGHTGRAY "lightgray"
#define C_WHITE "white"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include <sys/ioctl.h>
#include <signal.h>
#ifdef HAVE_SLANG_H
#include <slang.h>
#else
#ifdef HAVE_SLANG_SLANG_H
#include <slang/slang.h>
#endif
#endif
#else
extern int sl_screen_width, sl_screen_height, sl_screen_size_changed;
#endif

#define HEIGHT (sl_screen_height)
#define WIDTH (sl_screen_width)
enum {
	sl_key_esc = 27,
	sl_key_first = 0x100,
	sl_key_up, sl_key_down, sl_key_left, sl_key_right,
	sl_key_pgup, sl_key_pgdn, sl_key_del, sl_key_home,
	sl_key_end, sl_key_f0 = 0x200
};
#define sl_key_f(a) (sl_key_f0 + (a))

enum {
Black = 1,Grey,
White,Lwhite,
Green,Lgreen,
Cyan,Lcyan,
Red,Lred,
Blue,Lblue,
Mag,Lmag,
Brown,Yellow,

Active,
Active2,
Passive,
}; 


void sl_cls(void);
void sl_addstr_fill_alt(const char *s, int n, int c1, int c2);
void sl_addstr_alt(const char *s, int n, int c1, int c2);
void sl_init(void (*program_finish)(void));
void sl_goto(int x, int y);
void sl_frame(int x, int y, int w, int h);
void sl_putch(int c);
void sl_putchxy(int c, int x, int y);
void sl_addstr(const char *c);
void sl_addstrn(const char *c, int n);
void sl_addstrn_fill(const char *c, int n);
void sl_addstrxy(char *s, int x, int y);
int sl_strlen_alt(const char *s);
void sl_color(int c);
void sl_refresh(void);
void sl_fini(void);
void sl_die(const char *msg);
void sl_fill(int c, int x, int y, int w, int h);
int sl_getch2(void);
int sl_getch(void);
#endif
