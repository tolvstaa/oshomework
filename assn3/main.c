#include <stdio.h>

#define IPBUF_SIZE 2048

int main(int argc, char** argcv) {
	const char ipbuf[IPBUF_SIZE]; // set input buffer
	memset(ipbuf, 0, IPBUF_SIZE); // zero buffer contents

	while(fgets(ipbuf, IPBUF_SIZE, stdin) != EOF) {
		//printf(
	}

	return 0;
}
