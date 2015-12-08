#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, const char** argv) {
	srand(time(NULL));
	int x, i, j=atoi(argv[1]);
	if(argv[1]) for(i=0;i<j;i++) printf("%c",((x=rand()%27)==26)?32:x+65);
    printf("\n");
	return 0;
}
