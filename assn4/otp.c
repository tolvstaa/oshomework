#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>

#if !(defined(CRYPT) && (CRYPT==0 || CRYPT==1))
#error "Crypt must be set to 0 for encryption or 1 for decryption."
#define CRYPT -1
#endif

#define ENCRYPT 0
#define DECRYPT 1

void errexit(const char* inp) {
    if(inp) fprintf(stderr, "Error: %s", inp);
    else fprintf(stderr, "Usage: otp_enc/dec <filepath> <keypath> <port>\n");
    exit(1);
}

char* stringread(const char* filename) {
    FILE* file = fopen(filename, "r");
    if(!file) {
        fprintf(stderr, "Error: Could not open file '%s'.\n", filename);
        exit(1);
    }
    size_t size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    
    char* buffer = (char*) malloc((size+1)*sizeof(char));
    fgets(buffer, size, file);
    
    char* c;
    if((c = strchr(buffer, '\n'))) *c = '\0'; // remove newline
    return buffer;
}

int valid_chars(const char* input) {
    for(const char* c=input; *c; c++)
        if((*c<'A' || *c>'Z') && *c != ' ')
            return 0;
    return 1;
}

void spacesub(char* input, int crypt) {
    for(char* c = input; *c; c++)
        if(crypt == ENCRYPT) {
            if(*c == ' ') *c = '[';
        } else if(crypt == DECRYPT) {
            if(*c == '[') *c = ' ';
        }
}

// ./otp_enc <filepath> <keypath> <port>
int main(int argc, char** argv) {
    if(argc < 4) errexit(NULL);
    
    char* msg = stringread(argv[1]);
    char* key = stringread(argv[2]);

    if(!valid_chars(msg)) errexit("Invalid characters in message.\n");
    if(!valid_chars(key)) errexit("Invalid characters in key.\n");
    
    size_t keylen = strlen(key);
    size_t msglen = strlen(msg);
    if(keylen < msglen) errexit("Key not long enough.\n");
    
    int outlen = keylen + msglen + 2;
    char* output = (char*) malloc((outlen+1)*sizeof(char));

#if (CRYPT==ENCRYPT)
    sprintf(output, "%s<%s>", msg, key);
#elif (CRYPT==DECRYPT)
    sprintf(output, "%s>%s<", msg, key);
#endif
    free(msg);
    free(key);

    spacesub(output, ENCRYPT);
    unsigned int port;
    if(argv[3]) port = atoi(argv[3]); // port passed from command line
    else exit(1);

	int sendsock = socket(AF_INET, SOCK_STREAM, 0);
	if(sendsock==-1) errexit("Could not open socket.\n");

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	//server.sin_addr.s_addr = INADDR_ANY;
	
    if(connect(sendsock, (const struct sockaddr *) &server, sizeof(server))) {
        errexit("Could not connect to socket.\n");
    }
    
    send(sendsock, &outlen, sizeof(int32_t), MSG_MORE);
    send(sendsock, output, outlen*sizeof(char), 0);
    free(output);
    
    char* result = (char*) malloc((msglen+1)*sizeof(char));
    recv(sendsock, result, msglen, 0);
    if(!strcmp(result, "x")) errexit("Tried to connect to wrong daemon.\n");
    spacesub(result, DECRYPT);
    printf("%s\n", result);
    free(result);
    return 0;
}
