#ifndef SKIER_H
#define SKIER_H
struct s_hill;
struct s_skier;
struct s_skier {
	// pionter to current hill;
	struct s_hill *hill;
	// current frame, it realty cannot be set by state variable
	struct frame *current_frame;
	// pointer to next skier
	struct s_skier *next;
	// skier`s name
	char *name;
	// what skier`s doing now ?
	int state;
	// variables used to localize skier at the hill
	int x, y, current_x, current_y;
	double vx, vy, px, py;
	// variables used to judge skier	
	double alight, points, last_points, judge[5];
	// used only for cpu
	int level;
	// skier`s bend, (changed by arrows)
	int bend;
	// skier`s colors
	int color, color2;
	// some strange i realy don`t know what it is ? ugly code !
	int in_use, new_owner;
	// user id, (used in network mode)
	int id;
};

#ifdef SKIER_C
#include <string.h>
#include "hill.h"
#include "frame.h"
#include "slangwrap.h"
#include "xfnc.h"
#include "skin.h"
#include "main.h"

#define PREV_GRASP (-001)
#define NEXT_GRASP (-002)
#define NEXT_FRAME (-003)
static int sk_animate(struct s_skier *s, int state);
static void sk_setpoints(struct s_skier *s);
static void sk_service_lucky(struct s_skier *s);
#else
extern struct s_skier *sk_head, *sk_tail;
extern struct s_skier **sk_array;
extern int sk_counter;
#endif
void sk_array_sort(void);
void sk_array_init(int len);
struct s_skier *sk_init(struct s_hill *m, struct frame *f, char *name);
struct s_skier *sk_rewind(int n);
void sk_change_mode(struct s_skier *s, char *name, int color, int level, int in_use);
void sk_flush(struct s_skier *s, struct s_hill *m);
void sk_draw(struct s_skier *s);
void sk_add(struct s_skier *s);
int sk_service(struct s_skier *s, int key);
void sk_cpujump(struct s_skier *s);
#endif
