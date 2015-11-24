#ifndef AUX
#define AUX

typedef struct {
	char* argv[512];
	int argc;
} command;

void newln_comment_strip(char*);
void child_death(int);
void cmdshrink(command*, int);
int cmdfile(command*,const char*, char**, int);
command* parse(char*);
void unparse(command*);
int freopen_or_die(const char*, const char*, FILE*, command*);

#endif
