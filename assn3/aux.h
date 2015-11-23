#ifndef AUX
#define AUX

#define CF_IN 0
#define CF_OUT 1

typedef struct {
	char* argv[512];
	int argc;
} command;

void newln_comment_strip(char*);
char* cmdfile(command*,const char*);
command* parse(char*);
void unparse(command*);

#endif
