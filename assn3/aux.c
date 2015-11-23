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
