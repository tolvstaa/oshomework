CC=clang
CFLAGS=-g
EXE=tolvstaa.adventure

all: main

main: tolvstaa.adventure.c
	clear
	$(CC) tolvstaa.adventure.c -o $(EXE) $(CFLAGS)

run: main
	./$(EXE)

debug: main
	gdb -ex run ./$(EXE)

clean:
	rm -f $(EXE)
