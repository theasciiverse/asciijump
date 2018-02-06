/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) michal moskal, malekith@pld-linux.org
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define SLANG_C
#include "slangwrap.h"

int sl_screen_height, sl_screen_width;
void (*sl_program_finish)(void);

static void init_colors()
{
	SLtt_set_color(Black, 0, C_BLACK, C_BLACK);
	SLtt_set_color(Grey, 0, C_GRAY, C_BLACK);	
	SLtt_set_color(Blue, 0, C_BLUE, C_BLACK);
	SLtt_set_color(Lblue, 0, C_LIGHTBLUE, C_BLACK);
	SLtt_set_color(Green, 0, C_GREEN, C_BLACK);
	SLtt_set_color(Lgreen, 0, C_LIGHTGREEN, C_BLACK);
	SLtt_set_color(Cyan, 0, C_CYAN, C_BLACK);

	SLtt_set_color(Lcyan, 0, C_LIGHTCYAN, C_BLACK);
	SLtt_set_color(Red, 0, C_RED, C_BLACK);	
	SLtt_set_color(Lred, 0, C_LIGHTRED, C_BLACK);	
	SLtt_set_color(Mag, 0, C_MAGENTA, C_BLACK);
	SLtt_set_color(White, 0, C_LIGHTGRAY, C_BLACK);
	SLtt_set_color(Lwhite, 0, C_WHITE, C_BLACK);	
	
	SLtt_set_color(Yellow, 0, C_YELLOW, C_BLACK);	
	SLtt_set_color(Brown, 0, C_BROWN, C_BLACK);	
	
	SLtt_set_color(Active, 0, C_BLACK, C_WHITE);
	SLtt_set_color(Active2, 0, C_BLACK, C_WHITE);
	SLtt_set_color(Passive, 0, C_LIGHTGRAY, C_BLACK);
}


int sl_screen_size_changed;

static void resize_handler(int sig)
{
	struct winsize size;
	
	if (ioctl(1, TIOCGWINSZ, &size) == 0) {
		SLtt_Screen_Rows = sl_screen_height = size.ws_row;
		SLtt_Screen_Cols = sl_screen_width = size.ws_col;
		sl_screen_size_changed++;
	}

	signal(sig, resize_handler);
}

static void int_handler(int x)
{
	(void)x;
	sl_die("interrupt");
	
	if (sl_program_finish)
		sl_program_finish();
}

void sl_cls()
{
	SLsmg_cls();
}

void sl_addstr_fill_alt(const char *s, int n, int c1, int c2)
{
	sl_color(c1);
	
	while (*s && n--) {
		if (*s == '_' && s[1]) {
			sl_color(c2);
			sl_putch(*++s);
			sl_color(c1);
			s++;
		} else
			sl_putch(*s++);
	}

	sl_color(c1);
	while (n-- > 0)
		sl_putch(' ');
}

void sl_addstr_alt(const char *s, int n, int c1, int c2)
{
	sl_color(c1);
	
	while (*s && n--) {
		if (*s == '_' && s[1]) {
			sl_color(c2);
			sl_putch(*++s);
			sl_color(c1);
			s++;
		} else
			sl_putch(*s++);
	}
	
	sl_color(c1);
}


void sl_init(void (*program_finish)(void))
{
	SLtt_get_terminfo();
	if (SLkp_init() == -1)
		sl_die("Unable to initialize SLkp.");
		
	if (SLang_init_tty(-1, 0, 0) == -1)
		sl_die("Unable to initialize SLang terminal.");
	SLsmg_init_smg();

	/* enable UTF-8 mode if using appropiate locale */
	SLutf8_enable(-1);
	SLtt_utf8_enable(-1);
	SLsmg_utf8_enable(-1);
	SLinterp_utf8_enable(-1);
	
	SLang_set_abort_signal(int_handler);

	init_colors();

	sl_screen_height = SLtt_Screen_Rows;
	sl_screen_width = SLtt_Screen_Cols;
	sl_program_finish = program_finish;

	resize_handler(SIGWINCH);
}

void sl_goto(int x, int y)
{
	SLsmg_gotorc(y, x);
}

void sl_frame(int x, int y, int w, int h)
{
	SLsmg_draw_box(y, x, h, w);
}

void sl_putch(int c)
{
	SLsmg_write_char(c & 0xff);
}

void sl_putchxy(int c, int x, int y)
{
	sl_goto(x, y);
	sl_putch(c);
}

void sl_addstr(const char *c)
{
	SLsmg_write_string((char*)c);
}

void sl_addstrxy(char *s, int x, int y)
{
	sl_goto(x, y);
	sl_addstr(s);
}

void sl_addstrn(const char *c, int n)
{
	SLsmg_write_nstring((char*)c, n);
}

void sl_addstrn_fill(const char *c, int n)
{
	int k;
	
	if ((k = strlen(c)) > n)
		SLsmg_write_nchars((char*)c, n);
	else {
		SLsmg_write_string((char*)c);
		SLsmg_write_nstring(0, n - k);
	}
}

void sl_color(int c)
{
	SLsmg_set_color(c);
}

void sl_refresh()
{
	SLsmg_refresh();
}

void sl_fini()
{
	SLsmg_reset_smg();
	SLang_reset_tty();
}

void sl_die(const char *msg)
{
	sl_fini();
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

void sl_fill(int c, int x, int y, int w, int h)
{
	SLsmg_fill_region(y, x, h, w, c);
}

int sl_strlen_alt(const char *s)
{
	int n;
	
	for (n = 0; *s; s++, n++) 
		if (*s == '_' && s[1])
			s++;
	return n;
}

int sl_getch()
{
	int k;

	if (SLang_input_pending(0) <= 0)
		return 0;
	
	k = SLkp_getkey();

	switch (k) {
	
	case SL_KEY_ERR:
		return 0;
	case SL_KEY_UP:
		return sl_key_up;
	case SL_KEY_DOWN:
		return sl_key_down;
	case SL_KEY_LEFT:
		return sl_key_left;
	case SL_KEY_RIGHT:
		return sl_key_right;
	case SL_KEY_PPAGE:
		return sl_key_pgup;
	case SL_KEY_NPAGE:
		return sl_key_pgdn;
	case SL_KEY_BACKSPACE:
	case 8:
		return '\b';
	case '\r':
	case SL_KEY_ENTER:
		return '\n';
	case SL_KEY_DELETE:
		return sl_key_del;
	case SL_KEY_HOME:
		return sl_key_home;
	case SL_KEY_END:
		return sl_key_end;
	default:
		if (k < 256)
			return k;
		if (k > SL_KEY_F0 && k < SL_KEY_F0 + 24)
			return sl_key_f(k - SL_KEY_F0);
		return 0;
	}
}

int sl_getch2()
{
	int oldkey = 0, key = 0;
	while ((key = sl_getch()))
		oldkey = key;
	return oldkey;
}
