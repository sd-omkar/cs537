#file to make executables
CC=gcc
CFLAGS=-lmem -L.

all: mem mymain
mem: mem.c
	$(CC) -c -fpic mem.c -Wall
	$(CC) -shared -o libmem.so mem.o

mymain: mymain.c
	$(CC) $(CFLAGS) mymain.c -o myprogram -Wall
clean:
	rm *.o
