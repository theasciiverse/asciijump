/* :: asciijump (server), gnu gpl v 2
   :: copyright (c) grzegorz moskal, eldevarth@nemerle.org */
   
#define AS_DISPLAY_C
#include "as_display.h"

int as_allow_colored_prompt = 1;
int as_debug = 0;

void as_colored_prompt(char *prompt)
{
	int c, h = 1, c2 = -1, h2 = 0;
	printf("\r");
	
	if (as_allow_colored_prompt == 0) {
		if (strcmp(prompt, PROMPT) == 0) 
			printf("%s",PROMPTPREFIX);
			
		printf("%s", prompt);
		return;
	}
	
	if (strcmp(prompt, ERR) == 0) {
		c = Black;
		h = 1;
		c2 = Red;
	} else if (strcmp(prompt, DEBUG) == 0) {
		c = Green;
		h = 1;
		c2 = Green;
		h2 = 0;
	} else if (strcmp(prompt, MSG) == 0) {
		c = Black;
		h = 1;
		c2 = White;
		h2 = 0;
	} else if (strcmp(prompt, PROMPT) == 0) {
		c = Cyan;
		c2 = None;
		h = 0;
		h2 = 0;
		printf("[0m[%d;%dm%s", (h == 0), c, PROMPTPREFIX);
	} else if (strcmp(prompt, LOGO) == 0) {
		c = Black;
		h = 1;
		c2 = Brown;
	}
	
	
	else 
		c = None;

	if (c2 == -1)
		c2 = c;
	
	printf("[0m[%d;%dm%s[0m[%d;%dm",h, c, prompt, h2, c2);
	fflush(stdout);
}

void as_display(char *prompt, char *fmt, ...)
{
	va_list ap;
	char *s;
	int d;
	int arg = 0;

	if (strcmp(prompt, DEBUG) == 0 && as_debug == 0)
		return;
		

	va_start(ap, fmt);
	as_colored_prompt(prompt);
	for (; *fmt; fmt++) 
		switch (*fmt) {
		case '%': 
			arg = 1;
			continue;
		case 's':        
			if (arg) {
				s = va_arg(ap, char *);
				printf("%s", s);
				arg = 0;
			} else
				putchar(*fmt);
			break;
		case 'd':           
			if (arg) {
				d = va_arg(ap, int);
				printf("%d", d);
				arg = 0;
			} else 
				putchar(*fmt);
			break;
		case '\n':
			printf("\n");
			as_colored_prompt(prompt);
			break;
		default:
			putchar(*fmt);
		}
		
	va_end(ap);
	printf("\n");
	if (strcmp(prompt, LOGO))
		as_prompt();
}

void as_eerr(char *text, int status)
{
	printf("system error>> ");
	perror(text);
	printf("\n");
	fflush(stdout);
	exit(status);
}

void as_prompt(void)
{
	as_colored_prompt(PROMPT);
	fflush(stdout);
}
