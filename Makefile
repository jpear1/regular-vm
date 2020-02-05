CC=gcc
CFLAGS= -g -fsanitize=address -fsanitize=undefined 

BINARIES= interactiveCalc emulator ass dsm

all: dsm ass emulator

test: all
	./ass test.reg
	./dsm test.out

emulator: emulator.c
	$(CC) $(CFLAGS) -o $@ $^

dsm: disassembler.c
	$(CC) $(CFLAGS) -o $@ $^

ass: assembler.c
	$(CC) $(CFLAGS) -o $@ $^ 

# THIS WILL NOT COMPILE WITH $(CFLAGS) BECAUSE IT IS BAD
interactiveCalc: interactiveCalc.c calc.c utility.c intStack.c stringStack.c
	$(CC) -o $@ $^ 

clean:
	rm $(BINARIES)