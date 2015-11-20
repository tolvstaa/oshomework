CC=clang
CFLAGS=-g 
EXE=smallsh

all: clear main aux
	$(CC) main.o aux.o -o $(EXE)


main: main.c
	$(CC) $(CFLAGS) main.c -c

aux: aux.c aux.h
	$(CC) $(CFLAGS) aux.c -c



run: all
	./$(EXE)

debug: all
	gdb -q -ex run ./$(EXE)

clear:
	clear

clean:
	rm -f $(EXE) *.o
