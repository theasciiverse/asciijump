#ifndef SKIN_H
#define SKIN_H
enum {
	START, 
	DESCENT, 
	RIDE, 
	JUMP, 
	FLY, 
	TELEMARK,
	DESCENT2,
	RIDE2,
	STOP,
	CRASH,
	FINISH
};
#ifdef  SKIN_C
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "frame.h"
#include "xfnc.h"
static struct frame *skin_init(char *dirname);
#endif
struct frame *skin_make(char *prefix);
#endif
