/* :: asciijump (client), gnu gpl v 2
   :: copyright (c) grzegorz moskal, g.moskal@opengruop.org */

#define XFNC_C
#include "xfnc.h"

char *xgetcwd()
{
	char *dir = NULL;
	int size = 100;

	dir = malloc(size);
	
	while (getcwd(dir, size) == NULL && errno == ERANGE)
		dir = xrealloc(dir, size *=2);
	return dir;
}

DIR *xopendir(char *name)
{
	DIR *dir;
	if (name == NULL || (dir = opendir(name)) == NULL) {
		return NULL;
//		perror("opendir() faild ::");
//		fprintf(stderr, "Can`t open dir: %s\n", name);
//		exit(2);
	}
	return dir;
}

void xchdir(char *name)
{
	if (name == NULL || chdir(name) == -1) {
		perror("chdir() failed ::");
		fprintf(stderr, "Can`t change dir: %s\n", name);
		exit(2);
	}
}

void *xmalloc(size_t size)
{
	void *ptr;
	
	if (!size)
		return NULL;

	if ((ptr = calloc(1, size)) == NULL) {
		perror("calloc() failed ::");
		exit(1);
	}
	
	return ptr;
}

void *xrealloc(void *ptr, size_t size)
{

	if (!size)
		return NULL;
	
	ptr = realloc(ptr, size);
	
	if (ptr == NULL) {
		perror("realloc() failed ::");
		exit(1);
	}
	return ptr;
}

void *xfree(void *ptr)
{
	if (ptr)
		free(ptr);
	return 0;
}

FILE *xfopen(char *name, char *mode)
{
	FILE *fd = fopen(name, mode);
	if (fd == NULL) {
		perror("fopen() failed :: ");
		fprintf(stderr, "Can`t open file: %s\n", name);
		exit(3);
	}
	return fd;
}

int d(int h)
{
	if (h)
		return rand() % h +1;
	return 0;
}

void rand_init()
{
	srand(time(0));
}

char *xread(int fd)
{
	int length = 1, i = 0;
	char *line = NULL;
	size_t sofc = sizeof(char);

	for (;;) {
		if (length-1 == i) {
			length *=3;
			line = xrealloc(line, length*sofc);
		}
		 
		read(fd, &line[i], sofc);
//		if (read(fd, &line[i], sofc) == -1) 
//			perror(">> read faild ");
			

		if (line[i] == 0 || line[i] == '\0') {
			if (i == 0)
				return NULL;
			line[i] = '\0';
			break;

		} else if (line[i] == '\n') {
			if (i == 0)
				return xread(fd);
			line[i] = '\0';
			break;
		}
		i++;
	}
	
	return line;
}
char *xgetline(FILE *fd)
{
	int length = 1;
	int i = 0;
	char *line = NULL;

	for (;;) {
		if (length-1 == i) {
			length *=3;
			line = xrealloc(line, length*(sizeof(char)));
		}
		
		line[i] = fgetc(fd);

		if (line[i] == EOF) {
			if (i == 0)
				return NULL;
			line[i] = '\0';
			break;

		} else if (line[i] == '\n') {
			if (i == 0)
				return xgetline(fd);
			line[i] = '\0';
			break;
		}
		i++;
	}
	
	return line;
}

char *strglue(const char *s1, const char *s2, const char *s3)
{
	char *r;
	
	if (s1 == NULL)
		return NULL;
	if (s2 == NULL)
		s2 = "";
	if (s3 == NULL)
		s3 = "";

	r = malloc(sizeof(char) * (strlen(s1) + strlen(s2) + strlen(s3) +1));
	sprintf(r,"%s%s%s", s1, s2, s3);

	return r;
}

char *strdglue(const char *p1, const char *p2)
{
	return strglue(p1, "/", p2);
}

char *strwide(char *s)
{
	char *wt = xmalloc(sizeof(char)*(strlen(s)*2+1));
	char *j = wt;
	while (s && *s) {
		*j++ = *s++;
		*j++ = ' ';
	}
	*j = '\0';
	return wt;
}

char *n_type_description(n_type t)
{
	switch (t) {
	case Msg:
		return "message";
	case Hill:
		return "hill";
	case Results:
		return "result";
	case User:
		return "user data";
	case Bye:
		return "bye signal";
	case You:
		return "this user data";
	case Id:
		return "user id";
	case Everyone:
		return "everyone done";
	case Kick:
		return "karma police";
	}
}


char *hill_name;
int hill_length;

// should also read line like this:' name = foo ', TODO ;)
static int hill_readconf(char *filename)
{
	FILE *fd = fopen(filename, "r");
	char *line = NULL;

	if (fd == NULL)
		return 0;
	
	while ((line = xgetline(fd))) {
		if (strstr(line, "name=")) {
			if (hill_name)
				xfree(hill_name);

			hill_name = strdup(&line[5]);
		}
			
		if (strstr(line, "length=")) 
			hill_length = atoi(&line[7]);

		xfree(line);
	}

	if (hill_length == 0 && hill_name == NULL)	
		return 0;
		
	return 1;
}

int hill_readdir_extend(char *dirname, int *counter_ptr, void (*fnc)(void))
{
	
	struct dirent *j;
	char *currentdir = xgetcwd();
	DIR *where = xopendir(dirname);
	int counter = 0;
	int i = 0;

	if (where == NULL)
		return -1;
		
	xchdir(dirname);
	
	while ((j = readdir(where))) {
		if (*j->d_name == '.' || strcmp("core", j->d_name) == 0)
			continue;
		
		hill_length = 0;

		if (hill_name)
			hill_name = xfree(hill_name);

		if (hill_readconf(j->d_name) == 0)
			continue;
		fnc();
		counter++;
	}
	
	xchdir(currentdir);
	xfree(currentdir);
	*counter_ptr += counter;
	
	return counter;
}
