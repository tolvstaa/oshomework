#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>

#define DIRPERMS S_IRWXU | S_IRGRP | S_IROTH

#define NUM_NAMES	10
#define NUM_ROOMS	7
#define NAME_LEN	7
#define TYPE_LEN	11
#define CONN_MIN	3
#define CONN_MAX	6

#define DIRNAME_LEN 32
#define FPATH_BUF_LEN 64

const char NAMES[NUM_NAMES][NAME_LEN] = {"Room A","Room B","Room C",
	"Room D","Room E","Room F","Room G","Room H","Room I","Room J"};


typedef struct Room {
	struct Room* connections[CONN_MAX];
	int cn_count;
	char name[NAME_LEN];
	char type[TYPE_LEN];
} Room;

typedef struct Graph { // TODO: refactor to not use a struct here
	Room data[NUM_ROOMS];
} Graph;



int rn_conns(int n) {
	// returns random int, CONN_MIN-CONN_MAX inclusive
	srand(time(NULL)+n);
	return rand()%(CONN_MAX-CONN_MIN)+(CONN_MAX-(CONN_MAX-CONN_MIN));
}



void init_room(Room* r, const char name[NAME_LEN], int count, const char type[TYPE_LEN]) {
	strcpy(r->name, name);
	r->cn_count = count;
	strcpy(r->type, type);
	memset(r->connections, 0, CONN_MAX*sizeof(Room*));
}



void init_graph(Graph* g) {
	int i,j,m;
	
	// randomize names
	int write;
	char r_names[NUM_ROOMS][NAME_LEN];				// random names
	
	for(i=0;i<NUM_ROOMS;i++) {						// for each slot in new array
		srand(time(NULL)+i);
		m = rand() % NUM_NAMES;						// pick random name from NAMES

		write = 0;
		while (!write && i != 0) {					// while name not inserted or errored out, skipping first
			for (j=0;j<i;j++) {						// for existing names in new array
				write = 1;
				if(!strcmp(NAMES[m], r_names[j])) {	// if random name already exists
					m=++m%NUM_NAMES;				// circular increment new room name
					write = 0;
					break;							// start scanning existing names again
				}
			}
		}											// success
		strcpy(r_names[i], NAMES[m]);				// name DNE in new array yet, insert name
	}
	
	// printer for r_names
	/* for(i=0;i<NUM_ROOMS;i++)
		printf("%s, ",r_names[i]);
	printf("\b\b.\n"); */

	// define rooms
	init_room(&(g->data[0]), r_names[0], rn_conns(0), "START_ROOM");		// start room
	for(i=1;i<NUM_ROOMS-1;i++)
		init_room(&(g->data[i]), r_names[i], rn_conns(i), "MID_ROOM");		// mid rooms
	init_room(&(g->data[NUM_ROOMS-1]), r_names[NUM_ROOMS-1], rn_conns(NUM_ROOMS-1), "END_ROOM");	// end room

	// TODO connect rooms
}

void import_graph(Graph* g, char* dirname) {
	DIR* d = opendir(dirname);
	// TODO Read all data back into new graph and use graph for game logic
	closedir(d);
}



void populate_file(char* fname, Room r) {
	FILE* o = fopen(fname,"w");
	fprintf(o,"ROOM NAME: %s\n",r.name);
	
	int i;
	for (i=0;i<r.cn_count;i++) {
		if(r.connections[i]) // FIXME
			fprintf(o,"CONNECTION %d: %s\n",i+1,r.connections[i]->name);
		else
			fprintf(o,"ERROR: NULL CONNECTION\n");
	}
	
	fprintf(o,"ROOM TYPE: %s\n", r.type);
	fclose(o);
}

void gen_roomfiles(char* dirname) {
	int i;
	Graph g;
	char buffer[FPATH_BUF_LEN];
	
	init_graph(&g);
	
	for(i=0;i<NUM_ROOMS;i++) {
		sprintf(buffer, "%s/room%d", dirname, i);
		populate_file(buffer, g.data[i]);
	}
}



int main(int argc, char** argv) {
	char dirname[DIRNAME_LEN];
	sprintf(dirname, "tolvstaa.rooms.%d", getpid());
	
	mkdir(dirname, DIRPERMS); // make directory
	gen_roomfiles(dirname); // generate all room files
	
	// Graph g;
	// import_graph(&g, dirname);
	
	// TODO user interaction
	
	return 0;
}
