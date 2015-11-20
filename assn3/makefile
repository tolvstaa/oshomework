CC=clang
CFLAGS=-g
EXE=smallsh

all: main
	$(CC) main.o aux.o -o $(EXE)

main: main.c
	$(CC) $(CFLAGS) main.c -c

aux: aux.c aux.h
	$(CC) $(CFLAGS) aux.c -c

run: all
	./$(EXE)

debug: main
	gdb -q -ex run ./$(EXE)

clean:
	rm -f $(EXE) *.o
