#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#include "./aux.h"
#define BUF_SIZE 2048


int main(int argc, char** argcv) {
	char ipbuf[BUF_SIZE];
	command* c; 
	int status, prev_status, bgs;

	while(printf(": "), fflush(stdout), fgets((char*)ipbuf, BUF_SIZE, stdin)) {
		newln_comment_strip((char*)ipbuf); // strip newline from input
		c = parse(ipbuf);
		
		if(c->argc) {
			if(!strcmp(c->argv[0], "exit")) { // handle exit
				unparse(c); // deallocate command
				kill(0, SIGTERM); // kill process group
				return 0; // exit shell
			} else if(!strcmp(c->argv[0], "cd" )) { // handle cd
				prev_status = chdir((c->argc>1)?c->argv[1]:getenv("HOME"));
				if(prev_status) fprintf(stderr, "Could not change to directory \"%s\"\n", c->argv[1]);
			} else if(!strcmp(c->argv[0], "status")) { // handle status
				printf("%d\n", prev_status);
				prev_status = 0;
				
			} else { // handle binaries
				int bg = !strcmp(c->argv[c->argc-1], "&"); // if background proc
				if(bg) cmdshrink(c, 1);

				pid_t bgpid, pid = fork();
				if(pid == 0) { // if child
					char* inf = NULL, * otf = NULL;
					int ird = cmdfile(c, "<", &inf, bg); // input file redir
					int ord = cmdfile(c, ">", &otf, bg); // output file redir
					if(freopen_or_die(inf, "r", stdin, c)) return 1; // redirect stdin
					if(freopen_or_die(otf, "w", stdout, c)) return 1; // redirect stdout
					cmdshrink(c, 2*ird + 2*ord); // remove redirs
					
					execvp(c->argv[0], c->argv); // execute
					fprintf(stderr, "%s: command not found.\n", c->argv[0]); // failed exec
					unparse(c); // deallocate command
					return 1; // exit shell

				} else if(!bg) { // if parent of non-background child
					waitpid(pid, &status, 0);
					prev_status = WEXITSTATUS(status);
				
				} else { // if parent of background child
					printf("background pid is %d\n", pid);
				}
			}
		}
		unparse(c); // deallocate command
		
		pid_t pid = waitpid(0,&bgs, WNOHANG); // background process status
		if(pid > 0) {
			printf("background pid %d is done: ", pid);
			if(WIFSIGNALED(bgs)) printf("terminated by signal %d\n", WTERMSIG(bgs));
			else printf("exit status %d\n", WEXITSTATUS(bgs));
		}
	}
	return 0;
}
