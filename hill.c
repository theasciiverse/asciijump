/* :: asciijump (client), gnu gpl v 2
      copyright (c) 2000, 2003 
      grzegorz moskal (eldevarth), g.moskal@opengruop.org
      copyright (c) 2000 
      michal moskal (malekith), malekith@pld-linux.org
   :: przemyslaw niezborala (misiek), finburson@o2.pl			*/

#define HILL_C
#include "hill.h"

struct s_hill *hl_head, *hl_tail;
int hl_counter = 0;

struct s_hill* hl_init(int len, char *name)
{
	struct s_hill *h = XALLOC(s_hill);
	
	h->name = strdup(name);
	h->k = len*2;
	
	len += 10;
	len *= 4;
	len += 180;

	h->len = len;

	// hl_fill(h) not called here 
	
	return h;
}


struct s_hill* hl_find(char *pattern, struct s_hill *head)
{
	struct s_hill *h = head;
	
	if (h == NULL)	
		h = hl_head;
	
	for (; h; SWITCH(h))
		if (strcmp(h->name, pattern) == 0)
			break;
	return h;
}

void hl_add(struct s_hill *h)
{
	if (hl_head == NULL)
		hl_tail = hl_head = h;
	else {
		hl_tail->next = h;
		SWITCH(hl_tail);
	}
}

/* where hill records live: VARDIR on a setgid install, else ~/.asciijump/
   (created if needed), so records keep without setgid games */
static const char *hl_record_dir(void)
{
	static char *dir = NULL;
	char *home;

	if (dir)
		return dir;
	if (access(VARDIR, W_OK) == 0 || (home = getenv("HOME")) == NULL)
		return dir = VARDIR;
	dir = strglue(home, "/.asciijump/", "");
	mkdir(dir, 0755);
	return dir;
}

int hl_record_write(struct s_hill *h)
{
	char *name = strglue(hl_record_dir(), h->name, "");
	FILE *fd = fopen(name, "w");
	
	xfree(name);
	
	if (fd == NULL)
		return 0;

	fwrite(&h->record, sizeof(double), 1, fd);	
	fclose(fd);

	name = strglue(hl_record_dir(), h->name, ".owner");
	fd = fopen(name, "w");
	xfree(name);
	if (fd == NULL)
		return 0;
	fprintf(fd, "%s", h->owner);
	fclose(fd);

	return 1;
}

void hl_record_read(struct s_hill *h)
{
	char *name = strglue(hl_record_dir(), h->name, "");
	FILE *fd = fopen(name, "r");
	xfree(name);

	if (fd == NULL) 
		return;
		
	fread(&h->record, sizeof(double), 1, fd);
	fclose(fd);
	
	name = strglue(hl_record_dir(), h->name, ".owner");
	fd = fopen(name, "r");
	xfree(name);
	
	if (fd == NULL) {
		h->owner = strdup("unknow");
		return;
	}
	
	h->owner = xgetline(fd);
	fclose(fd);
}

static void hl_service_adding(void)
{
	struct s_hill *h = hl_init(hill_length, hill_name);
	
	hl_record_read(h);
	hl_add(h);
}

int hl_readdir(char *dirname)
{
	return hill_readdir_extend(dirname, &hl_counter, hl_service_adding);
}


static int ix2(int x, double p)
{
	return (int)(p * x * x);
}

static void hl_fill(struct s_hill *hl)
{
	int *place;
	int len = hl->len;

	int l1, l2, l3;
	int p, k, i;
	double w1 = 0.0033, w2 = 0.0034, w3 = 0.0025;
	
	hl->fill_no = xmalloc(sizeof(int)*(len));
	for (i = 0; i < len; i++)
		hl->fill_no[i] = d(10) - 1;

	place = hl->caption = xmalloc(sizeof(int) * (hl->len + 1));
	
	for (i = 0; i < 60; i++)
		*place++ = 0;
	len -= 180;
	
	l1 = len * 12 / 33;
	l2 = len * (12 + 9) / 33;
	l3 = len;
	p = len / 33;

	i = 0;

	for (; i < l1; i++)
		place[i] = ix2(-l1 + i, w1);
	hl->leapsill = i + 60;		

	for (; i < l2; i++)
		place[i] = -ix2(i - l1, w2) - p;

	k = -place[i - 1] + ix2(-l3 + i - 1, w3);
	
	for (; i < l3; i++)
		place[i] = ix2(-l3 + i, w3) - k;

	p = place[0] - place[i - 1];
	k = -place[i - 1];

	for (i = 0; i < len; i++)
		place[i] = p - (place[i] + k);
		
	for (i = l1; i > l1 -l1/3; i--)
		if (place[i] == place[i-6])
			hl->pns = (i-9) + 60;
			
	for (i = l3; i > l3 - l3/3; i--)
		if (place[i] == place[i-6])
			hl->pnz = (i-6) +60;
			
	for (p = place [l3-1], i = l3; i < l3+120; place[i++] = p);
	
}

static char fill[10][10] = {
	".%#<==-,-.",
	",*#===-(-.",
	",#=#==---.",
	".8#===-.-.",
	".&#=:=---.",
	",#=:==:_-.",
	",@#==`-~-.",
	".H#===`--.",
	".#====-^`.",
	",*=/==--'."
};

/* TODO */

void hl_draw(struct s_skier *sk)
{
	struct s_hill *hl = sk->hill;
	int offset_x = sk->x - WIDTH/2;
	int offset_y;
	int scr_x, scr_y, dd, px;

	if (hl->caption == NULL)
		hl_fill(hl);
		
	if (sk->x == 62)
		sk->y = hl->caption[sk->x];
		
	offset_y = sk->y - HEIGHT/2;
		
	sl_color(Lwhite);	
	
	if (offset_x < 0) 
		offset_x = 0; 
		
	for (scr_y = 0; scr_y < HEIGHT-1; scr_y++) {
		for (scr_x = 0; scr_x < WIDTH; scr_x++) {
			// printed x
			px = offset_x + scr_x;
			
			if (((dd = hl->caption[px] - (scr_y + offset_y))) >= 0)
				continue;

			sl_goto(scr_x, scr_y);
			
			if (dd > -10) { 
				if (px == hl->k + hl->leapsill && dd != -1) {
					sl_color(Red + d(2)-1);
					sl_putch('|');
				} else if (px == (int)(2*hl->record + hl->leapsill)
					&& dd != -1) {
					sl_color(Green + d(2)-1);
					sl_putch('|');
				} else {
				 	sl_color(Lwhite);
					sl_putch(fill[hl->fill_no[px]][-dd - 1]);
				}
			} else if (dd == -10 && px > hl->leapsill + hl->k/2 
				&& px < hl->pnz) {
				if (px % 10 == 7) {
					char tab[4]; 
					sprintf(tab, "%d", (px - hl->leapsill)/2);
					sl_color(Red);
					sl_addstrxy(tab, scr_x-2, scr_y);
					sl_color(Lwhite);
				} else
					sl_putch('-');
			} else {
				sl_color(White);
				sl_putch('\"');
			}
		}
	}
		
	sl_color(Cyan);
	sl_addstrxy(hl->name, WIDTH/2-strlen(hl->name)/2, HEIGHT-1);

/*	if (sk->new_owner) {
		sl_color(Brown);
		sl_addstrxy("that`s new hill record !", WIDTH/2-10, HEIGHT/2-6);
	}
*/
}

