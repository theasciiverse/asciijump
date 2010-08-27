/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define SKIER_C
#include "skier.h"
#define OX ((K/2 < 100) ? wgp1[s->bend].ox : wgp2[s->bend].ox)
#define OY ((K/2 < 100) ? wgp1[s->bend].oy : wgp2[s->bend].oy)
#define SN ((K/2 < 100) ? wgp1[s->bend].sn : wgp2[s->bend].sn)
#define K  (s->hill->k)
// tail and head for main skiers list
struct s_skier *sk_head, *sk_tail;
// array for s_skiers
struct s_skier **sk_array;
int sk_counter;
// typical names for famous skiers ;)
static char *sk_names[] = {
"player", "next player", "next ^2 player", "next ^3 player",
"erykah badu","charlie parker", "thom yorke", "bjork", NULL
};

void sk_array_sort()
{
	struct s_skier *s;
	int i;
	
	for (i = 1; sk_array[i] && i < sk_counter; i++) {
	
		if (sk_array[i]->points >= sk_array[i-1]->points) 
			continue;
			
		s = sk_array[i-1];
		sk_array[i-1] = sk_array[i];
		sk_array[i] = s;
		
		i = 0;
	}
}

void sk_add(struct s_skier *s)
{
	if (sk_head == NULL)
		sk_tail = sk_head = s;
	else {
		sk_tail->next = s;
		SWITCH(sk_tail);
	}
}

struct s_skier *sk_rewind(int n)
{
	struct s_skier *s = sk_head;
	
	while (n-- > 0 && s)
		SWITCH(s);
		
	return s;
}

void sk_array_init(int len)
{
	int i;
	struct s_skier *s;

	sk_counter = len;
	sk_head = sk_tail = NULL;	// do not free memory, 
	
	for (i = 0; i < sk_counter; i++) {
		s = sk_init(hl_head, skin, (i < 8) ? sk_names[i] : "foo");
		if (i < 4)
			s->level = 0;
		sk_add(s);
	}
	
	sk_array = xmalloc(sizeof(struct object *)*sk_counter);
}

struct s_skier* sk_init(struct s_hill *hl, struct frame *f, char *name)
{
	struct s_skier *s = XALLOC(s_skier);
	
	s->current_frame = f;
	
	s->color = Lblue;
	s->color2 = Cyan;
	
	sk_flush(s, hl);
	sk_change_mode(s, name, 1 + d(8), d(3), 0);
	
	return s;
}

void sk_change_mode(struct s_skier *s, char *name, int color, int level, int in_use)
{
	s->color = color;
	s->in_use = in_use;
	s->level = level;
	if (name) {
		xfree(s->name);
		s->name = strdup(name);
	}
}
void sk_flush(struct s_skier *s, struct s_hill *m)
{
	s->hill = m;
	s->x = 62;
//	s->y = m->caption[s->x]-1;	// here caption is 0x0 
	sk_animate(s, START);

	s->current_x = WIDTH/2-4;		// TODO
	s->current_y = HEIGHT/2-2;		// it cannot be store in every skier !
	
	s->vx = 0.0; 
	s->vy = 0.0;
	s->px = 0.0;
	s->py = 0.0;

	s->bend = 0;
}


void sk_draw(struct s_skier *s)
{
	char result[20];
	
	fr_draw(s->current_frame, s->current_x, s->current_y, 
		"oO0@-/\\_", s->color, s->color2);
	
	sl_color(Green);
	sl_goto(0, HEIGHT-1);
	sl_addstr(s->name);

	if (s->state > TELEMARK) {
		int i = 0;
		sprintf(result, "%.2f _m  %.1f", s->alight, s->last_points);
		sl_goto(WIDTH-strlen(result)-1, HEIGHT-1);
		sl_addstr_alt(result, sl_strlen_alt(result), Cyan, Lcyan);
		for (; i < 5; i++) {
			sl_goto(WIDTH-7, 2*i+1);
			sprintf(result, "%1.1f ", s->judge[i]);
			sl_addstr_alt(result, sl_strlen_alt(result), Cyan, Lcyan);
		}
	} else {
		if (s->hill->owner)
			sprintf(result, "%10.10s - %3.2f", s->hill->owner,
				s->hill->record);
		else
			sprintf(result, "%10.10s - 0.0 m", "none");
		sl_goto(WIDTH-strlen(result)-7, HEIGHT-1);
		sl_color(Green);
		sl_addstr("record: ");
		sl_addstr(result);
	}
}

static int sk_animate(struct s_skier *s, int state)
{
	struct frame *f = s->current_frame;
	if (state == NEXT_FRAME) {
		for (; f; SWITCH(f))
			if (f->state != s->current_frame->state)
				break;
		s->current_frame = f;
	} else if (state == NEXT_GRASP) {
		if (s->current_frame->next && 
			s->current_frame->next->state ==
			s->current_frame->state) 
			SWITCH(s->current_frame);
		else
			return 0;
	} else if (state == PREV_GRASP) {
		if (s->current_frame->prev && 
			s->current_frame->prev->state ==
			s->current_frame->state) 
			PREV(s->current_frame);
		else
			return 0;
	} else {
		for (; f->prev; PREV(f));
		for (; f; SWITCH(f))
			if (f->state == state) 
				break;
		s->current_frame = f;
	}
	
	s->state = s->current_frame->state;
	return 1;
}

static void sk_setpoints(struct s_skier *s)
{
	double unit, points = 60;
	double lowest = 100, bigest = -2;
	int k = K/2, i;
	
	if (s->state == CRASH)
		for (i = 0; i < 5; i++) 
			s->judge[i] = -7;
		
	
	if (k >= 150) {
		unit = 1.2;
		points = 120;
	} else if (k >= 100)
		unit = 1.8;
	else if (k >= 50)
		unit = 2;
	else
		unit = 2.1;
		
	if (s->alight > k)
		for (; k <= s->alight; k++)
			points += unit;
	else if (s->alight < k)
		for (; k >= s->alight; k--)
			points -= unit;

	for (i = 0; i < 5; i++) {
		if (s->alight < K/2)
			s->judge[i] -= 3;
		if (s->bend == 3)
			s->judge[i] -= 1;
		s->judge[i] += 20;
		s->judge[i] -= d(s->bend)/d(2);
		if (d(2)-1)
			s->judge[i] -= 0.5;
		if (lowest > s->judge[i])
			lowest = s->judge[i];
		if (bigest < s->judge[i])
			bigest = s->judge[i];
	}

	for (i =0; i < 5; i++) {
		if (s->judge[i] == bigest) 
			bigest = -1;
		else if (s->judge[i] == lowest)
			lowest = -1;
		else
			points += s->judge[i];
	}
	
	s->points += points;
	s->last_points = points;

	if (s->hill->record < s->alight && s->state != CRASH) {
		s->hill->record = s->alight;
		xfree(s->hill->owner);
		s->hill->owner = strdup(s->name);
		s->new_owner = 1;
		hl_record_write(s->hill);
	}
}

static void sk_service_crash(struct s_skier *s)
{
	if (s->hill->caption[s->x] - s->y > 0) 
		return;
		
	sk_animate(s, CRASH);
	s->alight = (s->x - s->hill->leapsill)/2;
	sk_setpoints(s);
}

static void sk_service_telemark(struct s_skier *s)
{
	int i;
	int r = s->hill->caption[s->x] - s->y;

	if (r > 8 || r < 0) {
		sk_service_crash(s);
		return;
	}
		
	if (r <= 1 && d(10) < s->bend) 
		return;

	sk_animate(s, TELEMARK);
	s->alight = (s->x - s->hill->leapsill)/2 + 
		((s->bend == 1) ? d(2): d(2)-1);
	
	if (d(13) ==  7)
		s->alight++;
	else if (d(13) == 13)
		s->alight--;
			
	for (i = 0; i < 5; i++) 
		s->judge[i] = 0;
	s->alight += (0.01 * d(100));
	sk_setpoints(s);
}

static void sk_service_jump(struct s_skier *s)
{
	double px = 0;
	int mvy = 0;

	if (s->x <= s->hill->leapsill -5) {
		px = 0.7/d(7);
		mvy = d(2);
	} else if (s->x <= s->hill->leapsill - 4) {
		px = 0.6/d(9);
		mvy = 1+d(2);
	} else if (s->x <= s->hill->leapsill - 2) {
		px = 0.8/d(9);
		mvy = 2 + d(2);
	} else if (s->x >= s->hill->leapsill) {
		px = 0.6/d(2);
	}
		
	s->bend = 2;

	s->vx = 2.3 + K/100 - px;
	s->y -= mvy;
	
	if (K/2 >= 100) {
		s->vx -= (K/2-99) * 0.013;
		s->y -= mvy-2;
	}

	s->vy = K/230;
	s->px = s->x;
	s->py = s->y;
	
	sk_animate(s, JUMP);
}

struct opory {
	double ox, oy ,sn;
} wgp1[] = {
	{ 3, 40, 2 },
	{ 0, 0, 1 },
	{ 3, 3, 2 },
	{ 3, 10, 4.6 }
},wgp2[] = {
	{ 8, 30, 6 },
	{ 0, 0, 1 },
	{ 10, 8, 5 },
	{ 3, 5, 3.2 },
};

void sk_service_move(struct s_skier *s)
{
	if (s->state == FLY || s->state == JUMP) {
		const double w1 = 1000, w2 = 200, G = 0.098; 
		double ax, ay = G;

		ax = -(OX * s->vx) / w1;
		ay = G + (OY * s->vy) / w1 - (SN * s->vx) / w2;

		s->vx += ax;
		s->vy += ay;
		s->px += s->vx;
		s->py += s->vy;

		s->x = (int)s->px;
		s->y = (int)s->py;
		
		if (d(50) == -7)
			sk_service_lucky(s);
			
	} else if (s->state != START && s->state != FINISH) {
		s->y = s->hill->caption[s->x];
		s->vx += 65/K;
		
		if (s->state == RIDE || s->state == RIDE2)
			s->vx += 0.1;
		s->x+= s->vx;
	} else
		s->vx = 0;
}

static void sk_service_lucky(struct s_skier *s)
{
	if (K/2 > 100)
		s->vx=2.5;
	else if (K/2 > 70)
		s->vx=3;
	else
		s->vx=3.5;
		
	s->vy=1;
}

void sk_cpujump(struct s_skier *s)
{
	int fail = 0, i;
	double k = K/2;
	
	s->bend = d(4)-1;
	
	for (i = 0; i < 5; i++) 
		s->judge[i] = 0;
		
	switch (s->level) {
	case 1:
		fail = 7;
		k -= k/3 + k/4;
		break;
	case 2:
		fail = 3;
		k -= k/4;
		break;
	case 3:
		k -= k/4;
		fail = 1;
		s->bend = d(2)-1;
		break;
	}

	if (d(10) <= fail)
		k -= K/2;
	else
		k += (d(s->level) ? 3 : 2) * d(K/10);
		
	s->alight = k;
	sk_setpoints(s);
}

int sk_service(struct s_skier *s, int key)
{
	sk_service_move(s);
	
	if (key == 'q' && s->state > TELEMARK)
		s->state = FINISH;

	switch (s->state) {
	case START:
		if ((key == '\n' || s->current_frame->prev) &&
			sk_animate(s, NEXT_GRASP) == 0) {
			s->vx = 2;
			sk_animate(s, DESCENT);
		}
		break; 
	case DESCENT:
		sk_animate(s, NEXT_GRASP);
		if (s->x >= s->hill->pns)
			sk_animate(s, RIDE);
		break; 
	case RIDE:
		sk_animate(s, NEXT_GRASP);
		if (key == '\n' || s->x >= s->hill->leapsill)
			sk_service_jump(s);
		break; 
	case JUMP:
		if (sk_animate(s, NEXT_GRASP) == 0) {
			sk_animate(s, FLY);
			sk_animate(s, NEXT_GRASP);
			sk_animate(s, NEXT_GRASP);
		}
		break; 
	case FLY:
		sk_service_crash(s);
		if (key == sl_key_left && s->bend > 0) {
			sk_animate(s, PREV_GRASP);	
			s->bend--;
		} else if (key == sl_key_right && s->bend < 3) {
			sk_animate(s, NEXT_GRASP);	
			s->bend++;
		} else if (key == '\n') 
			sk_service_telemark(s);
		else if (key == '\'') 
			sk_service_lucky(s);
		
		break; 
		
	case TELEMARK:
		if (sk_animate(s, NEXT_GRASP) == 0) {
			if (s->x < s->hill->pnz)
				sk_animate(s, DESCENT2);
			else 
				sk_animate(s, RIDE2);
		}
		break;
	case DESCENT2:
		sk_animate(s, NEXT_GRASP);
		if (s->x >= s->hill->pnz)
			sk_animate(s, RIDE2);
		break;
	case RIDE2:
		sk_animate(s, NEXT_GRASP);
		if (s->x > s->hill->len-s->current_x-25)
			sk_animate(s, STOP);
		
		break;
	case STOP:
		if (sk_animate(s, NEXT_GRASP) == 0 &&
			s->x > s->hill->len-s->current_x-5)
			s->state = FINISH;
		break;
	case CRASH:
		sk_animate(s, NEXT_GRASP);
		if (s->x > s->hill->len-s->current_x-15)
			s->state = FINISH;
		break;
	case FINISH:
		if (s->new_owner)
			s->new_owner = 0;
		if (key)
			return 0;
	}
	return 1;
}
