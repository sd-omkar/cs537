CC=gcc

EXE=fastsort
INPUT=input
OUTPUT=output
SEED=26
NUM=200

CFLAGS=-O3 -o $(EXE)

.PHONY: all clean run generate dump

all:
	$(CC) $(CFLAGS) $(EXE).c

clean:
	rm -f $(EXE) 

run:
	./$(EXE) -i $(INPUT) -o $(OUTPUT)

generate:
	./generate -s $(SEED) -n $(NUM) -o $(INPUT)

dump:
	./dump -i $(INPUT) > $(INPUT).txt
	./dump -i $(OUTPUT) > $(OUTPUT).txt
