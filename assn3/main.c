#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "./aux.h"

#define BUF_SIZE 2048


int main(int argc, char** argcv) {
	char ipbuf[BUF_SIZE];
	command* c; 
	int prev_status;

	while(printf(": "), fgets((char*)ipbuf, BUF_SIZE, stdin)) {
		newln_comment_strip((char*)ipbuf); // strip newline from input
		c = parse(ipbuf);
		
		printf("cmd >> %s\n", c->argv[0]);
		printf("arg1 >> %s\n", c->argv[1]);
		printf("arg2 >> %s\n", c->argv[2]);
		printf("arg3 >> %s\n", c->argv[3]);
		printf("argc >> %d\n", c->argc);

		if(!strcmp(c->argv[0], "exit")) { // handle exit
			printf("Exiting...\n");
			return 0;
		}
		else if(!strcmp(c->argv[0], "cd" )) { // handle cd
			prev_status = chdir(c->argv[1]);
			if(prev_status) { printf("Could not change to directory \"%s\"\n", c->argv[1]); }
		}
		else if(!strcmp(c->argv[0], "status")) { // handle status
			printf("%d\n", prev_status);
			prev_status = 0;
		}
		else { // handle binaries
			int pid = fork();
			if(pid == 0) {
				// TODO deal with files
				execvp(c->argv[0], c->argv);
				printf("%s: could not execute.\n", c->argv[0]);
				return 1;
			} else {
				waitpid(pid, &prev_status, 0);
				prev_status = WEXITSTATUS(prev_status);
			}
		}
		unparse(c);
	}
	return 0;
}


