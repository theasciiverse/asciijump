/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define INTRO_C
#include "intro.h"
#define TEST 
static int clear = 0;
int typewriter(char *str, int x, int y)
{
#ifdef TEST
	return 0;
#endif
	if (str == NULL)
		return 0;
		
	if (x == -1)
		x = sl_screen_width/2-strlen(str)/2;
		
	if (y == -1)
		y = sl_screen_height/2;
		
	if (clear)
		sl_cls();
		
	sl_color(White);
		
	for (sl_goto(x, y); *str; str++) {
		if (*str == '\n') {
			sl_goto(x, y++);
			continue;
		}
		
		if (sl_getch())
			return 0;
		sl_putch(*str);
		sl_refresh();
		usleep(17000);
	}
	
	if (clear)
		for (x = 1; x < 130; x++) {
			if (sl_getch())
				break;
			usleep(1000);
		}
	return 1;
}

void intro()
{
	char buf[100];
		
	sprintf(buf,"%s", strwide(PACKAGE));
	if (typewriter(buf, -1, -1) == 0)
		return;
			
	sprintf(buf,"%s  %s", strwide("version"), strwide(VERSION));
	if (typewriter(buf, -1, sl_screen_height/2+1) == 0)
		return;
			
	usleep(200*1000);
	clear = 1;
}

