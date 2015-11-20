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


command* parse(char* input) {
	
	// initialize variables
	command* ret = malloc(sizeof(command));
	char* temp;
	int i;
	memset(ret->argv, 512*sizeof(char*), 0);
	ret->argc = 0;
	
	// zero-length input
	if(!strlen(input)) {
		printf("Returning zero input.\n");
		return ret;
	}
	
	// initialize strtok
	temp = strtok(input, " ");
	ret->argv[0] = strdup(temp);
	printf("Token 0: %s\n", temp);
	
	// tokenize arguments
	for(i=1; (temp = strtok(NULL, " ")); i++) {
		printf("Token %d: %s\n",i, temp);
		ret->argv[i] = strdup(temp);
	}
	
	// set arg count
	ret->argc = i+1;

	return ret;
}

void unparse(command* c) {
	// deallocate command
	for(int i=0; i<c->argc; i++) {
		free(c->argv[i]);
	}
	free(c);
}
