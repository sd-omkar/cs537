CC=gcc

EXE=fastsort
INPUT=input
OUTPUT=output

CFLAGS=-O3 -o $(EXE)

all:
	$(CC) $(CFLAGS) $(EXE).c

clean:
	rm -f $(EXE) 

run:
	./$(EXE) -i $(INPUT) -o $(OUTPUT) 	
