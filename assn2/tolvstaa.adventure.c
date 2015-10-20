#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define DIRPERMS S_IRWXU | S_IRGRP | S_IROTH

void gen_roomfiles() {
	// make files for rooms
}

int main() {
	char dirname[32];
	sprintf(dirname, "tolvstaa.rooms.%d", getpid());

	mkdir(dirname, DIRPERMS); // make directory
	gen_roomfiles(); // generate all room files

	printf("Hello world, process ID is %d\n", getpid());
	
	// for file in directory
		// remove file
	rmdir(dirname); // remove directory
	return 0;
}
