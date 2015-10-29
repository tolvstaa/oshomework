#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define DIRPERMS S_IRWXU | S_IRGRP | S_IROTH

const char* NAMES[] = {"Room A","Room B","Room C","Room D","Room E",
					"Room F","Room G","Room H","Room I","Room J"};

typedef enum ROOM_TYPE {
	START,
	MID,
	END
} ROOM_TYPE;

typedef struct Room Room;
struct Room {
	Room** connections;
	int cn_count;
	char name[8];
	ROOM_TYPE type;
};

typedef struct Graph Graph;
struct Graph {
	Room* data;
	int size;
};


void init_graph(Graph* g, int n) {
	
}

populate_file(char* f, Room r) {
	FILE* o = fopen(f,"w");
	fprintf(o,"ROOM NAME: %s\n",r.name);
	
	int i;
	for (i=0;i<r.cn_count;i++)
		fprintf(o,"CONNECTION %d: %s\n",i+1,r.connections[i]);
	
	fprintf(o,"ROOM TYPE: ");
	switch(r.type) {
		case START	: fprintf(o,"START_ROOM");	break;
		case MID	: fprintf(o,"MID_ROOM");	break;
		case END	: fprintf(o,"END_ROOM");	break;
	}
	fclose(o);
}

void gen_roomfiles(char* dirname) {
	int i;
	Graph g;
	char buffer[64];

	init_graph(&g);

	for(i=0;i<g.size;i++) {
		sprintf(buffer, "%s/room%d", dirname, i);
		populate_file(buffer, g.data[i]);
	}
}

int main(int argc, char** argv) {
	char dirname[32];
	sprintf(dirname, "tolvstaa.rooms.%d", getpid());
	
	mkdir(dirname, DIRPERMS); // make directory
	gen_roomfiles(dirname); // generate all room files

	printf("Hello world, process ID is %d\n", getpid());
	return 0;
}
