CC=gcc
CFLAGS= -g -fsanitize=address -fsanitize=undefined 

BINARIES= interactiveCalc emu ass dsm rpp

all: dsm ass emu rpp

test: all
	./rpp test.reg
	./ass test.a
	./dsm test.out
	./emu test.out

emu: emu.c emulator.c disassembler.c
	$(CC) $(CFLAGS) -o $@ $^

dsm: dsm.c disassembler.c
	$(CC) $(CFLAGS) -o $@ $^

ass: ass.c assembler.c shared.c
	$(CC) $(CFLAGS) -o $@ $^

rpp: rpp.c preprocessor.c shared.c
	$(CC) $(CFLAGS) -o $@ $^

# THIS WILL NOT COMPILE WITH $(CFLAGS) BECAUSE IT IS BAD
interactiveCalc: interactiveCalc.c calc.c utility.c intStack.c stringStack.c
	$(CC) -o $@ $^

clean:
	rm $(BINARIES)