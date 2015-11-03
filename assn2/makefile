CC=gcc
CFLAGS=-g
EXE=tolvstaa.adventure

all: main

main: tolvstaa.adventure.c
	clear
	$(CC) $(CFLAGS) tolvstaa.adventure.c -o $(EXE)

run: main
	./$(EXE)

debug: main
	gdb -q -ex run ./$(EXE)

clean:
	rm -f $(EXE)
	rm -rf tolvstaa.rooms.*

print: clean run
	for x in tolvstaa.rooms.*/*; do cat $$x; echo ""; done
