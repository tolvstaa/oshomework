#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "aux.h"

// truncate newline and comment from string
void newln_comment_strip(char* c) {
	char* n;
	if((n = strchr(c, '\n'))) *n = '\0';
	if((n = strchr(c, '#'))) *n = '\0';
}

void cmdshrink(command* c, int n) {
	for(int i=0;i<n;i++) { // remove arguments
		free(c->argv[--c->argc]);
		c->argv[c->argc] = NULL;
	}
}

char* cmdfile(command* c, const char* token) { // return in/out filename or NULL
	for(int i=0;i<c->argc;i++)
		if(!strcmp(c->argv[i],token)) {
			printf("%sing to \"%s\"\n", token, c->argv[i+1]);
			return c->argv[i+1];
		}
	return NULL;
}

command* parse(char* input) {
	// initialize variables
	command* ret = malloc(sizeof(command));
	char* temp;
	int i;
	memset(ret->argv, 0, 512*sizeof(char*));
	ret->argc = 0;
	if(!strlen(input)) { return ret; } // zero-length input
	
	temp = strtok(input, " "); // initialize strtok
	ret->argv[0] = strdup(temp);
	for(ret->argc=1; (temp = strtok(NULL, " ")); ret->argc++) { // tokenize arguments
		ret->argv[ret->argc] = strdup(temp);
	}
	
	return ret;
}

void unparse(command* c) {
	// deallocate command
	for(int i=0; i<c->argc; i++) {
		free(c->argv[i]);
	}
	free(c);
}

int freopen_or_die(const char* f, const char* m, FILE* o, command* c) {
	if(f && !freopen(f, m, o)) {
		printf("Could not open file \"%s\".\n", f);
		unparse(c);
		return 1;
	}
	return 0;
}
