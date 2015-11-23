#ifndef AUX
#define AUX

typedef struct {
	char* argv[512];
	int argc;
} command;

void newln_comment_strip(char*);
void cmdshrink(command*, int);
char* cmdfile(command*,const char*);
command* parse(char*);
void unparse(command*);
int freopen_or_die(const char*, const char*, FILE*, command*);

#endif
