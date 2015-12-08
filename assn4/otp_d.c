#define _POSIX_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#if !(defined(CRYPT) && (CRYPT==0 || CRYPT==1))
    #error "Crypt must be set to 0 for encryption or 1 for decryption."
    #define CRYPT -1
#endif

#define ENCRYPT 0
#define DECRYPT 1

/* http://cs.uccs.edu/~cs522/pp/f99pp-4.gif*/
int parse(char* input, char** key) { // key should be like <this> for enc, >this< for dec
    char* c;
    for(c = input; *c != '<' && *c != '>'; c++);
    int mode = (*c == '>'); // 0=enc, 1=dec
    *c = '\0'; // set terminator for message
    
    *key = c+1;
    for(; *c != '<' && *c != '>'; c++);
    *c = '\0'; // set terminator for key
    return mode;
}

char* crypt(char* input, char* key) { // new memory with converted text
	char* output = (char*) malloc((strlen(input)+1)*sizeof(char)), * i, * j, * k;
	for(i = input, j = output, k = key; *i; i++, j++, k++)
		*j = ((*j = ((*i-'A') + (CRYPT?-1:1)*(*k-'A'))) < 0?*j+27:*j%27)+'A';
	strcat(output, "\0");
	return output;
}

int main(int argc, char** argv) {
    unsigned int port;
    if(argv[1]) port = atoi(argv[1]); // port passed from command line
    else exit(1);

	int listensock = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;
	
	bind(listensock, (struct sockaddr*) &server, sizeof(server));
	listen(listensock, 5);

	int i;
	pid_t pid;
	for(int i=0;i<5;i++) if(!(pid=fork())) break; // spawn 5 children

	for(;;) {
		if(!pid) { // child
            //printf("Child: New child PID %d, listening on %d\n", getpid(), port);
            int common_fd = accept(listensock, NULL, NULL); // blocking get client descriptor
            int inputsize; // read message sizes
            read(common_fd, &inputsize, sizeof(int32_t));
            char* input = malloc((sizeof(char)*(inputsize+1)));
            
            recv(common_fd, input, inputsize, 0); // get input
            
            char* key; // parse input
            if ((parse(input, &key)) != CRYPT) {
                printf("Child: Bad method in %scryptor child %d: given %scrypt job.\n",
                    (CRYPT?"de":"en"), getpid(), (CRYPT?"en":"de"));
                send(common_fd, "x", 2, 0);
                exit(1);
            }
            char* result = crypt(input, key); // en/decrypt
            free(input);

            // send back to client
            send(common_fd, result, strlen(result)+1, 0);
            free(result);

            exit(0); // phoenix death
		} else { // parent
            int status;
			pid_t cpid = wait(&status); // blocking call
			
            /*printf("Parent: ");
            if(WIFEXITED(status))
                printf("Child %d exited normally, with exit status %d.\n", cpid, WEXITSTATUS(status));
            else if(WIFSIGNALED(status) && WTERMSIG(status)==11)
                printf("Child %d suffered a segmentation fault.\n", cpid);
            else if(WIFSIGNALED(status))
                printf("Child %d was terminated with signal %d.\n", cpid, WTERMSIG(status));
			*/
            pid = fork();
		}
	}
	return 0;
}
