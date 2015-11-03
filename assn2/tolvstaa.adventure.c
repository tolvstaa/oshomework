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

#if TYPE_LEN > NAME_LEN
	#define DATA_LEN TYPE_LEN
#else
	#define DATA_LEN NAME_LEN
#endif

#define CONN_MIN	3
#define CONN_MAX	6

#define DIRNAME_LEN 32
#define FPATH_BUF_LEN 64
#define UIP_BUF_LEN 64

const char NAMES[NUM_NAMES][NAME_LEN] = {"Room A","Room B","Room C",
	"Room D","Room E","Room F","Room G","Room H","Room I","Room J"};

typedef struct Room {
	struct Room* conns[CONN_MAX];
	int cn_count;
	char name[NAME_LEN];
	char type[TYPE_LEN];
} Room;

typedef struct Graph {
	Room data[NUM_ROOMS];
} Graph;

void init_room(Room* r, const char name[NAME_LEN], int count, const char type[TYPE_LEN]) {
	strcpy(r->name, name);
	r->cn_count = count;
	strcpy(r->type, type);
	memset(r->conns, 0, CONN_MAX*sizeof(Room*));
}

static inline int rn_conns(int n) { // returns random int, CONN_MIN-CONN_MAX inclusive
	srand(time(NULL)+n);
	return rand()%(CONN_MAX-CONN_MIN)+(CONN_MAX-(CONN_MAX-CONN_MIN));
}

static inline Room** next_null(Room** s) { // searches through n pointers to find the closest null pointer
	int i;
	for(i=0;i<CONN_MAX;i++) if(!s[i]) return s+i;
	return NULL;
}

static inline int links_to(Room* src, Room* dest) {
	int i;
	for(i=0;i<CONN_MAX;i++) if(src->conns[i] == dest) return 1;
	return 0;
}

static inline int count_conns(Room** s) { // return a count of non-null connections
	int i;
	for(i=0;i<CONN_MAX;i++) if(!s[i]) return i;
	return CONN_MAX;
}

static inline Room* room_byname(Graph* g, const char* n) { // return address of room with name
	int i;
	for(i=0;i<NUM_ROOMS;i++) if(!strcmp(g->data[i].name, n)) return g->data+i;
	return NULL;
}

static inline Room* room_bytype(Graph* g, const char* t) { // return address of first room with type
	int i;
	for(i=0;i<NUM_ROOMS;i++) if(!strcmp(g->data[i].type, t)) return g->data+i;
	return NULL;
}

static inline int graph_enough_conns(Graph* g) { // return whether or not graph has at least CONN_MIN connections
	int i;
	for(i=0;i<NUM_ROOMS;i++) if(count_conns(g->data[i].conns) < CONN_MIN) return 0;
	return 1;
}
	
void init_graph(Graph* g) {
	int i,j,k,m;
	
	// randomize names
	int write;
	char r_names[NUM_ROOMS][NAME_LEN];				// random names
	for(i=0;i<NUM_ROOMS;i++) {						// for each slot in new array
		srand(time(NULL)+i);
		m = rand() % NUM_NAMES;						// pick random name from NAMES
		write = 0;
		while (!write && i != 0)					// while name not inserted or errored out, skipping first
			for (j=0;j<i;j++)						// for existing names in new array
				if(write = 1, !strcmp(NAMES[m], r_names[j])) {	// if random name already exists
					m=(m+1)%NUM_NAMES;				// circular increment new room name
					write = 0;
					break;							// start scanning existing names again
				}
		strcpy(r_names[i], NAMES[m]);				// name DNE in new array yet, insert name
	}
	
	do {
		// define rooms, zero connections
		init_room(g->data+0, r_names[0], rn_conns(0), "START_ROOM");		// start room
		for(i=1;i<NUM_ROOMS-1;i++)
			init_room(g->data+i, r_names[i], rn_conns(i), "MID_ROOM");		// mid rooms
		init_room(g->data+NUM_ROOMS-1, r_names[NUM_ROOMS-1], rn_conns(NUM_ROOMS-1), "END_ROOM");	// end room

		// connect rooms
		for(i=0;i<NUM_ROOMS;i++)														// for each room
			for(j=count_conns(g->data[i].conns);j<g->data[i].cn_count;j++)				// for each source slot
				for(k=(i+NUM_ROOMS/2)%NUM_ROOMS,m=0;m<NUM_ROOMS; k=(k+1)%NUM_ROOMS,m++)	// iterate dest circularly
					// except itself. if dest has open conns, and not already conn'd,
					if((i!=k)&&(count_conns(g->data[k].conns)<g->data[k].cn_count)&&(!links_to(g->data+i,g->data+k))){
						g->data[i].conns[j] = g->data+k;								// connect room to dest
						*(next_null(g->data[k].conns)) = g->data+i;						// connect dest to room
						break;															// stop looking for dests
					}
		for(i=0;i<NUM_ROOMS;i++)
		g->data[i].cn_count = count_conns(g->data[i].conns);							// correct any remaining nulls
	} while (!graph_enough_conns(g));
}

void data_import(Graph* g, const char* dirname, char mode) {
	int i, talley;
	char fpath_buf[FPATH_BUF_LEN], line_buf[16+DATA_LEN], field_buf[DATA_LEN];
	char* csi, * cdi;
	FILE* f;

	for(i=0;i<NUM_ROOMS;i++) { // import conns
		sprintf(fpath_buf, "%s/room%d", dirname, i);
		f = fopen(fpath_buf, "r");
		talley=0;

		while(fgets(line_buf, sizeof(line_buf), f)) {
			memset(field_buf, 0, sizeof(field_buf));
			for(csi=strstr(line_buf,": ")+2,cdi=field_buf;*csi;csi++,cdi++) // copy value to new buffer
				*cdi = *csi;
			if ((cdi=strchr(field_buf, '\n')) != NULL) *cdi = '\0'; // replace newline with terminator

			if(mode == 'n') // name mode
				if(strncmp(line_buf, "ROOM NAME", 9) == 0)
					strcpy(g->data[i].name, field_buf); // copy name
				else
					strcpy(g->data[i].type, field_buf); // copy type
			else { // connection mode
				if(strncmp(line_buf, "CONNECTION", 10) == 0) { // copy conns
					*(next_null(g->data[i].conns)) = room_byname(g, field_buf);
					talley++;
				}
				g->data[i].cn_count = talley; // set number of conns
			}
		}
		fclose(f);
	}
}

void import_graph(Graph* g, const char* dirname) {
	int i;
	for(i=0;i<NUM_ROOMS;i++) init_room(g->data+i,"",0,""); // init rooms to blank
	data_import(g, dirname, 'n'); // import names and types
	data_import(g, dirname, 'c'); // import conns and counts
}

void populate_file(char* fname, Room r) {
	FILE* o = fopen(fname,"w");
	int i;

	fprintf(o,"ROOM NAME: %s\n",r.name);	// print name
	for (i=0;i<r.cn_count;i++) {			// print connections
		if(r.conns[i])
			fprintf(o,"CONNECTION %d: %s\n",i+1,r.conns[i]->name);
		else
			fprintf(o,"CONNECTION %d: NULL\n",i+1);
	}
	fprintf(o,"ROOM TYPE: %s\n", r.type);	// print type
	fclose(o);
}

void gen_files(char* dirname) {
	int i;
	Graph g;
	char buffer[FPATH_BUF_LEN];
	
	init_graph(&g); // create a new graph
	for(i=0;i<NUM_ROOMS;i++) { // print each room to a file
		sprintf(buffer, "%s/room%d", dirname, i);
		populate_file(buffer, g.data[i]);
	}
}

int main(int argc, char** argv) {
	char dirname[DIRNAME_LEN];
	sprintf(dirname, "tolvstaa.rooms.%d", getpid());
	
	mkdir(dirname, DIRPERMS); // make directory
	gen_files(dirname); // generate all room files
	
	Graph g;
	import_graph(&g, dirname);
	
	Room* here = room_bytype(&g, "START_ROOM"), * next = NULL;
	int i, steps;
	char fpath_buf[FPATH_BUF_LEN], uip_buf[UIP_BUF_LEN], hist_buf[NAME_LEN];
	char* ci;
	sprintf(fpath_buf, "%s/log", dirname);
	FILE* log = fopen(fpath_buf,"w+");
	
	printf("(Psst... end room is %s)\n", room_bytype(&g,"END_ROOM")->name);
	while(strcmp(here->type, "END_ROOM")) {
		fprintf(log, "%s\n", here->name);
		printf("\nCURRENT LOCATION: %s\nPOSSIBLE CONNECTIONS: ", here->name);
		for(i=0;i<here->cn_count;i++)
		   printf("%s, ", here->conns[i]->name);
		printf("\b\b.\nWHERE TO? >");

		fgets(uip_buf, sizeof(uip_buf), stdin);
		if ((ci=strchr(uip_buf, '\n')) != NULL) *ci = '\0'; // replace newline with terminator
		for(i=0;i<here->cn_count;i++)
			if(next = here->conns[i])
				if(!strcmp(next->name, uip_buf))
					break;
				else
					next = NULL;

		if(next) {
			here = next;
			steps++;
		}
		else {printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n"); }
	}
	fprintf(log, "%s\n", here->name);
	rewind(log);
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
	while(fgets(hist_buf, sizeof(hist_buf), log)) printf(hist_buf);
	fclose(log);
	return 0;
}
