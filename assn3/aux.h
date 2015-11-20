#ifndef AUX
#define AUX

typedef struct {
	char* argv[512];
	int argc;
} command;

void newln_comment_strip(char*);
command* parse(char*);
void unparse(command*);

#endif
