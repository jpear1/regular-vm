CC=gcc
CFLAGS= -g -fsanitize=address -fsanitize=undefined 

BINARIES= interactiveCalc emu ass dsm

all: dsm ass emulator

test: all
	./ass test.reg
	./dsm test.out

emu: emu.c emulator.c disassembler.c
	$(CC) $(CFLAGS) -o $@ $^

dsm: dsm.c disassembler.c
	$(CC) $(CFLAGS) -o $@ $^

ass: ass.c assembler.c shared.c
	$(CC) $(CFLAGS) -o $@ $^

rpp: rpp.c preprocessor.c
	$(CC) $(CFLAGS) -o $@ $^

# THIS WILL NOT COMPILE WITH $(CFLAGS) BECAUSE IT IS BAD
interactiveCalc: interactiveCalc.c calc.c utility.c intStack.c stringStack.c
	$(CC) -o $@ $^

clean:
	rm $(BINARIES)