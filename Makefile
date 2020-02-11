CC=gcc
CFLAGS= -g -fsanitize=address -fsanitize=undefined -lgdbm
# THESE FLAGS DON'T WORK -fsanitize=address -fsanitize=undefined

BINARIES= interactiveCalc emu ass dsm rpp

vscode: all 

all: dsm ass emu rpp

test: all
	./rpp test.reg
	./ass test.a
	./dsm test.out

runTest: test
	./emu test.out

memTest: all
	valgrind ./rpp test.reg --leak-check=full

emu: emu.c emulator.c disassembler.c
	$(CC) -o $@ $^ $(CFLAGS)

dsm: dsm.c disassembler.c
	$(CC) -o $@ $^ $(CFLAGS)

ass: ass.c assembler.c shared.c
	$(CC) -o $@ $^ $(CFLAGS)

rpp: rpp.c preprocessor.c shared.c
	$(CC) -o $@ $^ $(CFLAGS)

# THIS WILL NOT COMPILE WITH $(CFLAGS) BECAUSE IT IS BAD
interactiveCalc: interactiveCalc.c calc.c utility.c intStack.c stringStack.c
	$(CC) -o $@ $^

clean:
	rm $(BINARIES)