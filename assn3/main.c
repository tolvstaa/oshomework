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
		
		if(c->argc) {
			if(!strcmp(c->argv[0], "exit")) { // handle exit
				unparse(c);
				return 0;
			} else if(!strcmp(c->argv[0], "cd" )) { // handle cd
				prev_status = chdir((c->argc>1)?c->argv[1]:getenv("HOME"));
				if(prev_status) printf("Could not change to directory \"%s\"\n", c->argv[1]);
			} else if(!strcmp(c->argv[0], "status")) { // handle status
				printf("%d\n", prev_status);
				prev_status = 0;
			} else { // handle binaries
				char bg = !strcmp(c->argv[c->argc-1], "&"); // if background proc
				if(bg) free(c->argv[--c->argc]); // strip '&' argument

				pid_t pid = fork();
				if(pid == 0) {
					char* inf = cmdfile(c, "<"); // input file redir
					char* otf = (bg)?"/dev/null":cmdfile(c, ">"); // output file redir
					if(inf && !access(inf, R_OK)) freopen(inf, "r", stdin);
					if(otf && !access(otf, W_OK)) freopen(otf, "w", stdout);

					execvp(c->argv[0], c->argv);
					fprintf(stderr, "%s: could not execute.\n", c->argv[0]);
					unparse(c);
					return 1;
				} else if(!bg) {
					waitpid(pid, &prev_status, 0);
					prev_status = WEXITSTATUS(prev_status);
				}
			}
		}
		unparse(c);
	}
	return 0;
}


