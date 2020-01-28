CC=gcc
CFLAGS= -g -fsanitize=address -fsanitize=undefined 

BINARIES=disassemblerTests interactiveCalc

tests: disassemblerTests
	./disassemblerTests


disassemblerTests: disassemblerTests.c disassembler.c
	$(CC) $(CFLAGS) -o $@ $^ 

# THIS WILL NOT COMPILE WITH $(CFLAGS) BECAUSE IT IS BAD
interactiveCalc: interactiveCalc.c calc.c utility.c intStack.c stringStack.c
	$(CC) -o $@ $^ 

clean:
	rm $(BINARIES)