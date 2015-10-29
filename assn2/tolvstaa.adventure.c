#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#define DIRPERMS S_IRWXU | S_IRGRP | S_IROTH
#define NUM_ROOMS 7

const char* NAMES[] = {"Room A","Room B","Room C","Room D","Room E",
					"Room F","Room G","Room H","Room I","Room J"};

typedef struct Room {
	struct Room** connections;
	int cn_count;
	char name[8];
	char type[11];
} Room;

typedef struct Graph {
	Room* data;
	int size;
} Graph;


int rn_rooms(int n) { // returns random int, 3-6 inclusive
	srand(time(NULL)+n);
	return rand()%3+3;
}

void init_room(Room* r, const char* name, int count, const char* type) {
	strcpy(r->name, name);
	r->cn_count = count;
	strcpy(r->type, type);
}

void init_graph(Graph* g, int n) {
	// allocate rooms
	g->size = n;
	g->data = malloc(sizeof(Room)*n);

	// define rooms
	char** r_names; // TODO scramble names

	init_room(&(g->data[0]), r_names[0], rn_rooms(0), "START_ROOM");		// start room
	int i;
	for(i=1;i<n-1;i++)
		init_room(&(g->data[i]), r_names[i], rn_rooms(i), "MID_ROOM");		// mid rooms
	init_room(&(g->data[n-1]), r_names[n-1], rn_rooms(n-1), "END_ROOM");	// end room

	// TODO connect rooms
}

void import_graph(Graph* g, char* dirname) {
	DIR* d = opendir(dirname);
	closedir(d);
}

void populate_file(char* fname, Room r) {
	FILE* o = fopen(fname,"w");
	fprintf(o,"ROOM NAME: %s\n",r.name);
	
	int i;
	for (i=0;i<r.cn_count;i++) fprintf(o,"CONNECTION %d: %s\n",i+1,r.connections[i]);
	
	fprintf(o,"ROOM TYPE: %s", r.type);
	fclose(o);
}

void gen_roomfiles(char* dirname) {
	int i;
	Graph g;
	char buffer[64];

	init_graph(&g, NUM_ROOMS);

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
	
	Graph g;
	// TODO Read all data back into new graph and use graph for game logic
	// import_graph(&g, dirname);

	return 0;
}
